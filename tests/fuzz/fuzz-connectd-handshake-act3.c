/* This is a fuzz test for Act 3 of the BOLT 8 handshake. We intercept io_read()
 * to inject the fuzzer-generated Act 3 packet in the handshake.
 *
 * The expected sequence of events for this test is:
 *   1. initiator calls io_write() with an Act 1 packet
 *     - we save the valid Act 1 packet and fail the handshake
 *   2. responder calls io_read() for the Act 1 packet
 *     - we inject the valid Act 1 packet from Step 1
 *   3. responder calls io_write() with an Act 2 packet
 *     - we discard the valid Act 2 packet
 *   4. responder calls io_read() for the Act 3 packet
 *     - we inject the fuzzer-generated packet
 *   5. responder fails to validate the packet
 */
#include "config.h"
#include <assert.h>
#include <ccan/io/io.h>
#include <ccan/mem/mem.h>
#include <common/utils.h>
#include <tests/fuzz/connectd_handshake.h>
#include <tests/fuzz/libfuzz.h>

/* Storage for the valid Act 1 packet extracted from the initiator. */
static u8 act_one[ACT_ONE_SIZE];

/* The io_write() interceptor.
 *
 * This should be called once when the initiator is writing out its Act 1
 * packet and once when the responder is writing out its Act 2 packet. */
static struct io_plan *
test_write(struct io_conn *conn, const void *data, size_t len,
	   struct io_plan *(*next)(struct io_conn *, struct handshake *),
	   struct handshake *h)
{
	++write_count;
	if (write_count == 1) {
		/* Initiator is sending the Act 1 packet. Save it for the
		 * responder and then error out. */
		assert(h->side == INITIATOR);
		assert(len == sizeof(act_one));
		memcpy(act_one, data, sizeof(act_one));

		return handshake_failed(conn, h);
	}
	if (write_count == 2) {
		/* Responder is sending the Act 2 packet. Check that it is
		 * initialized and discard it. */
		assert(h->side == RESPONDER);
		assert(len == ACT_TWO_SIZE);
		memcheck(data, len);

		return next(conn, h);
	}
	assert(false && "too many calls to io_write()");
}

/* The io_read() interceptor.
 *
 * This should be called once when the responder is reading the Act 1 packet and
 * once when the responder is reading the Act 3 packet. We inject fuzzer input
 * for the Act 3 packet. */
static struct io_plan *test_read(struct io_conn *conn, void *data, size_t len,
				 struct io_plan *(*next)(struct io_conn *,
							 struct handshake *),
				 struct handshake *h)
{
	++read_count;
	if (read_count == 1) {
		/* Responder is reading the Act 1 packet. Use the packet
		 * provided by the initiator. */
		assert(h->side == RESPONDER);
		assert(len == sizeof(act_one));
		assert(write_count == 1 &&
		       "initiator didn't send Act 1 packet");
		memcpy(data, act_one, sizeof(act_one));

		return next(conn, h);
	}
	if (read_count == 2) {
		/* Responder is reading the Act 3 packet. Provide the
		 * fuzzer-generated data as the packet. */
		assert(h->side == RESPONDER);
		assert(len == ACT_THREE_SIZE);
		assert(bytes_remaining >= len);
		memcpy(data, bytes, len);
		bytes += len;
		bytes_remaining -= len;

		return next(conn, h);
	}
	assert(false && "too many calls to io_read()");
}

void run(const u8 *data, size_t size)
{
	if (size < randombytes_SEEDBYTES + ACT_THREE_SIZE)
		return;

	init_globals(data, size);

	/* Do the initiator handshake up to Act 1 so we can extract the valid
	 * Act 1 packet. */
	handshake(INITIATOR);

	/* Do the responder handshake using the initiator's Act 1 packet and the
	 * fuzzer-generated Act 3 packet. */
	handshake(RESPONDER);

	clean_tmpctx();
}
