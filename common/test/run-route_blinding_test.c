#include "config.h"
#include "../amount.c"
#include "../bigsize.c"
#include "../blindedpath.c"
#include "../blinding.c"
#include "../features.c"
#include "../hmac.c"
#include "../json_parse.c"
#include "../json_parse_simple.c"
#include <ccan/tal/grab_file/grab_file.h>
#include <ccan/tal/path/path.h>
#include <common/channel_id.h>
#include <common/json_parse.h>
#include <common/json_stream.h>
#include <common/setup.h>
#include <common/wireaddr.h>
#include <stdio.h>

/* AUTOGENERATED MOCKS START */
/* Generated stub for fmt_channel_id */
char *fmt_channel_id(const tal_t *ctx UNNEEDED, const struct channel_id *channel_id UNNEEDED)
{ fprintf(stderr, "fmt_channel_id called!\n"); abort(); }
/* Generated stub for fromwire_channel_id */
bool fromwire_channel_id(const u8 **cursor UNNEEDED, size_t *max UNNEEDED,
			 struct channel_id *channel_id UNNEEDED)
{ fprintf(stderr, "fromwire_channel_id called!\n"); abort(); }
/* Generated stub for fromwire_node_id */
void fromwire_node_id(const u8 **cursor UNNEEDED, size_t *max UNNEEDED, struct node_id *id UNNEEDED)
{ fprintf(stderr, "fromwire_node_id called!\n"); abort(); }
/* Generated stub for fromwire_sciddir_or_pubkey */
void fromwire_sciddir_or_pubkey(const u8 **cursor UNNEEDED, size_t *max UNNEEDED,
				struct sciddir_or_pubkey *sciddpk UNNEEDED)
{ fprintf(stderr, "fromwire_sciddir_or_pubkey called!\n"); abort(); }
/* Generated stub for node_id_from_hexstr */
bool node_id_from_hexstr(const char *str UNNEEDED, size_t slen UNNEEDED, struct node_id *id UNNEEDED)
{ fprintf(stderr, "node_id_from_hexstr called!\n"); abort(); }
/* Generated stub for towire_channel_id */
void towire_channel_id(u8 **pptr UNNEEDED, const struct channel_id *channel_id UNNEEDED)
{ fprintf(stderr, "towire_channel_id called!\n"); abort(); }
/* Generated stub for towire_node_id */
void towire_node_id(u8 **pptr UNNEEDED, const struct node_id *id UNNEEDED)
{ fprintf(stderr, "towire_node_id called!\n"); abort(); }
/* Generated stub for towire_sciddir_or_pubkey */
void towire_sciddir_or_pubkey(u8 **pptr UNNEEDED,
			      const struct sciddir_or_pubkey *sciddpk UNNEEDED)
{ fprintf(stderr, "towire_sciddir_or_pubkey called!\n"); abort(); }
/* AUTOGENERATED MOCKS END */

static struct tlv_encrypted_data_tlv_payment_constraints *
json_to_payment_constraints(const tal_t *ctx, const char *buf, const jsmntok_t *t)
{
	struct tlv_encrypted_data_tlv_payment_constraints *pc
		= tal(ctx, struct tlv_encrypted_data_tlv_payment_constraints);

	assert(json_to_u32(buf, json_get_member(buf, t, "max_cltv_expiry"),
			   &pc->max_cltv_expiry));
	assert(json_to_u64(buf, json_get_member(buf, t, "htlc_minimum_msat"),
			   &pc->htlc_minimum_msat));
	return pc;
}

static struct tlv_encrypted_data_tlv_payment_relay *
json_to_payment_relay(const tal_t *ctx, const char *buf, const jsmntok_t *t)
{
	struct tlv_encrypted_data_tlv_payment_relay *relay
		= tal(ctx, struct tlv_encrypted_data_tlv_payment_relay);

	assert(json_to_u16(buf, json_get_member(buf, t, "cltv_expiry_delta"),
			   &relay->cltv_expiry_delta));
	assert(json_to_u32(buf, json_get_member(buf, t, "fee_proportional_millionths"),
			    &relay->fee_proportional_millionths));
	if (json_get_member(buf, t, "fee_base_msat"))
		assert(json_to_u32(buf, json_get_member(buf, t, "fee_base_msat"),
				   &relay->fee_base_msat));
	else
		relay->fee_base_msat = 0;
	return relay;
}

static u8 *json_to_allowed_features(const tal_t *ctx,
				    const char *buf, const jsmntok_t *t)
{
	size_t i;
	const jsmntok_t *f, *features = json_get_member(buf, t, "features");
	u8 *allowed_features;

	allowed_features = tal_arr(ctx, u8, 0);
	json_for_each_arr(i, f, features) {
		u32 n;
		assert(json_to_u32(buf, f, &n));
		set_feature_bit(&allowed_features, n);
	}
	return allowed_features;
}

static u8 *json_to_enctlvs(const tal_t *ctx,
			   const char *buf, const jsmntok_t *tlvs)
{
	struct tlv_encrypted_data_tlv *enctlv = tlv_encrypted_data_tlv_new(tmpctx);
	size_t i;
	const jsmntok_t *t;
	u8 *ret, *appended = tal_arr(tmpctx, u8, 0);

	json_for_each_obj(i, t, tlvs) {
		if (json_tok_streq(buf, t, "short_channel_id")) {
			enctlv->short_channel_id = tal(enctlv, struct short_channel_id);
			assert(json_to_short_channel_id(buf, t+1,
							enctlv->short_channel_id));
		} else if (json_tok_streq(buf, t, "padding")) {
			enctlv->padding = json_tok_bin_from_hex(enctlv,
								buf, t+1);
			assert(enctlv->padding);
		} else if (json_tok_streq(buf, t, "next_node_id")) {
			enctlv->next_node_id = tal(enctlv, struct pubkey);
			assert(json_to_pubkey(buf, t+1,
					      enctlv->next_node_id));
		} else if (json_tok_streq(buf, t, "path_id")) {
			enctlv->path_id = json_tok_bin_from_hex(enctlv,
								buf, t+1);
			assert(enctlv->path_id);
		} else if (json_tok_streq(buf, t, "next_blinding_override")) {
			enctlv->next_blinding_override = tal(enctlv, struct pubkey);
			assert(json_to_pubkey(buf, t+1,
					      enctlv->next_blinding_override));
		} else if (json_tok_streq(buf, t, "payment_relay")) {
			enctlv->payment_relay = json_to_payment_relay(enctlv, buf, t+1);
		} else if (json_tok_streq(buf, t, "payment_constraints")) {
			enctlv->payment_constraints = json_to_payment_constraints(enctlv, buf, t+1);
		} else if (json_tok_streq(buf, t, "allowed_features")) {
			enctlv->allowed_features = json_to_allowed_features(enctlv, buf, t+1);
		} else {
			u16 tagnum;
			u8 *val;
			assert(json_tok_startswith(buf, t, "unknown_tag_"));
			tagnum = atoi(buf + t->start + strlen("unknown_tag_"));
			assert(tagnum);
			val = json_tok_bin_from_hex(enctlv, buf, t+1);
			assert(val);

			/* We can't actually represent these in a way towire_
			 * will see, so we literally append them */
			towire_bigsize(&appended, tagnum);
			towire_bigsize(&appended, tal_bytelen(val));
			towire_u8_array(&appended, val, tal_bytelen(val));
		}
	}
	ret = tal_arr(ctx, u8, 0);
	towire_tlv_encrypted_data_tlv(&ret, enctlv);
	towire_u8_array(&ret, appended, tal_bytelen(appended));
	return ret;
}

int main(int argc, char *argv[])
{
	char *json;
	size_t i;
	jsmn_parser parser;
	jsmntok_t toks[5000];
	const jsmntok_t *t, *hops_tok;
	struct pubkey *ids;
	u8 **enctlvs;
	struct privkey blinding;

	common_setup(argv[0]);

	if (argv[1])
		json = grab_file(tmpctx, argv[1]);
	else {
		char *dir = getenv("BOLTDIR");
		json = grab_file(tmpctx,
				 path_join(tmpctx,
					   dir ? dir : "../bolts",
					   "bolt04/route-blinding-test.json"));
		if (!json) {
			printf("test file not found, skipping\n");
			goto out;
		}
	}

	jsmn_init(&parser);
	if (jsmn_parse(&parser, json, strlen(json), toks, ARRAY_SIZE(toks)) < 0)
		abort();

	hops_tok = json_get_member(json, json_get_member(json, toks, "generate"), "hops");
	ids = tal_arr(tmpctx, struct pubkey, hops_tok->size);
	enctlvs = tal_arr(tmpctx, u8 *, hops_tok->size);

	json_for_each_arr(i, t, hops_tok) {
		u8 *expected;
		assert(json_to_pubkey(json, json_get_member(json, t, "node_id"),
				      &ids[i]));
		enctlvs[i] = json_tok_bin_from_hex(enctlvs, json,
						   json_get_member(json, t, "encoded_tlvs"));
		expected = json_to_enctlvs(tmpctx, json,
					   json_get_member(json, t, "tlvs"));
		assert(tal_arr_eq(expected, enctlvs[i]));
	}

	/* Now do the blinding. */
	json_for_each_arr(i, t, hops_tok) {
		struct secret s;
		struct pubkey pubkey, expected_pubkey;
		u8 *enctlv, *expected_enctlv;
		struct pubkey alias, expected_alias;

		assert(json_to_secret(json,
				      json_get_member(json, t, "ephemeral_privkey"),
				      &s));

		/* First blinding/replacement is stated, remainder are
		 * derived! */
		if (json_get_member(json, t, "session_key"))
			assert(json_to_secret(json,
					      json_get_member(json, t, "session_key"),
					      &blinding.secret));
		else
			assert(secret_eq_consttime(&blinding.secret, &s));

		assert(pubkey_from_privkey(&blinding, &pubkey));
		json_to_pubkey(json, json_get_member(json, t, "ephemeral_pubkey"),
			       &expected_pubkey);
		assert(pubkey_eq(&pubkey, &expected_pubkey));

		enctlv = enctlv_from_encmsg_raw(tmpctx,
						&blinding,
						&ids[i],
						enctlvs[i],
						&blinding,
						&alias);
		expected_enctlv = json_tok_bin_from_hex(tmpctx,json,
							json_get_member(json, t,
									"encrypted_data"));
		assert(tal_arr_eq(enctlv, expected_enctlv));

		json_to_pubkey(json, json_get_member(json, t, "blinded_node_id"),
			       &expected_alias);
		assert(pubkey_eq(&alias, &expected_alias));
	}

out:
	common_shutdown();
}
