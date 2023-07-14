#include "config.h"
#include "../bech32.c"
#include "../json_filter.c"
#include "../json_parse.c"
#include "../json_parse_simple.c"
#include "../json_param.c"
#include <ccan/array_size/array_size.h>
#include <common/channel_type.h>
#include <common/setup.h>
#include <stdio.h>

char *fail_msg = NULL;
bool failed = false;

static bool check_fail(void) {
	if (!failed)
		return false;
	failed = false;
	return true;
}

struct command *cmd;

struct command_result {
};
static struct command_result cmd_failed;

struct command_result *command_fail(struct command *cmd,
				    enum jsonrpc_errcode code, const char *fmt, ...)
{
	failed = true;
	va_list ap;
	va_start(ap, fmt);
	fail_msg = tal_vfmt(cmd, fmt, ap);
	va_end(ap);
	return &cmd_failed;
}

/* AUTOGENERATED MOCKS START */
/* Generated stub for command_filter_ptr */
struct json_filter **command_filter_ptr(struct command *cmd UNNEEDED)
{ fprintf(stderr, "command_filter_ptr called!\n"); abort(); }
/* Generated stub for strip_lightning_prefix */
const char *to_canonical_invstr(const tal_t *ctx, const char *invstring UNNEEDED)
{ fprintf(stderr, "strip_lightning_prefix called!\n"); abort(); }
/* Generated stub for fromwire_tlv */
bool fromwire_tlv(const u8 **cursor UNNEEDED, size_t *max UNNEEDED,
		  const struct tlv_record_type *types UNNEEDED, size_t num_types UNNEEDED,
		  void *record UNNEEDED, struct tlv_field **fields UNNEEDED,
		  const u64 *extra_types UNNEEDED, size_t *err_off UNNEEDED, u64 *err_type UNNEEDED)
{ fprintf(stderr, "fromwire_tlv called!\n"); abort(); }
/* Generated stub for towire_tlv */
void towire_tlv(u8 **pptr UNNEEDED,
		const struct tlv_record_type *types UNNEEDED, size_t num_types UNNEEDED,
		const void *record UNNEEDED)
{ fprintf(stderr, "towire_tlv called!\n"); abort(); }
/* AUTOGENERATED MOCKS END */

/* We do this lightningd-style: */
enum command_mode {
	CMD_NORMAL,
	CMD_USAGE,
	CMD_CHECK
};

struct command {
	enum command_mode mode;
	bool deprecated_apis;
	const char *usage;
};

void command_set_usage(struct command *cmd, const char *usage)
{
	cmd->usage = usage;
}

bool command_usage_only(const struct command *cmd)
{
	return cmd->mode == CMD_USAGE;
}

bool command_check_only(const struct command *cmd)
{
	return cmd->mode == CMD_CHECK;
}

bool command_deprecated_apis(const struct command *cmd)
{
	return cmd->deprecated_apis;
}

struct json {
	jsmntok_t *toks;
	char *buffer;
};

static void convert_quotes(char *first)
{
	while (*first != '\0') {
		if (*first == '\'')
			*first = '"';
		first++;
	}
}

static struct json *json_parse(const tal_t * ctx, const char *str)
{
	struct json *j = tal(ctx, struct json);
	j->buffer = tal_strdup(j, str);
	convert_quotes(j->buffer);

	j->toks = tal_arr(j, jsmntok_t, 50);
	assert(j->toks);
	jsmn_parser parser;

      again:
	jsmn_init(&parser);
	int ret = jsmn_parse(&parser, j->buffer, strlen(j->buffer), j->toks,
			     tal_count(j->toks));
	if (ret == JSMN_ERROR_NOMEM) {
		tal_resize(&j->toks, tal_count(j->toks) * 2);
		goto again;
	}

	if (ret <= 0) {
		assert(0);
	}
	return j;
}

static void zero_params(void)
{
	struct json *j = json_parse(cmd, "{}");
	assert(param(cmd, j->buffer, j->toks, NULL));

	j = json_parse(cmd, "[]");
	assert(param(cmd, j->buffer, j->toks, NULL));
}

struct sanity {
	char *str;
	bool failed;
	int ival;
	u64 fpval; /* floating-point, multiplied by 1000000 */
	char *fail_str;
};

struct sanity buffers[] = {
	// pass
	{"['42', '3.15']", false, 42, 3150000, NULL},
	{"{ 'u64' : '42', 'fp' : '3.15' }", false, 42, 3150000, NULL},

	// fail
	{"{'u64':'42', 'fp':'3.15', 'extra':'stuff'}", true, 0, 0,
	 "unknown parameter"},
	{"['42', '3.15', 'stuff']", true, 0, 0, "too many"},
	{"['42', '3.15', 'null']", true, 0, 0, "too many"},

	// not enough
	{"{'u64':'42'}", true, 0, 0, "missing required"},
	{"['42']", true, 0, 0, "missing required"},

	// fail wrong type
	{"{'u64':'hello', 'fp':'3.15'}", true, 0, 0, "be an unsigned 64"},
	{"['3.15', '3.15', 'stuff']", true, 0, 0, "integer"},
};

static void stest(const struct json *j, struct sanity *b)
{
	u64 *ival;
	u64 *fpval;
	if (!param(cmd, j->buffer, j->toks,
		   p_req("u64", param_u64, &ival),
		   p_req("fp", param_millionths, &fpval), NULL)) {
		assert(check_fail());
		assert(b->failed == true);
		if (!strstr(fail_msg, b->fail_str)) {
			printf("%s != %s\n", fail_msg, b->fail_str);
			assert(false);
		}
	} else {
		assert(!check_fail());
		assert(b->failed == false);
		assert(*ival == 42);
		assert(*fpval > 3149900 && b->fpval < 3150100);
	}
}

static void sanity(void)
{
	for (int i = 0; i < ARRAY_SIZE(buffers); ++i) {
		struct json *j = json_parse(cmd, buffers[i].str);
		assert(j->toks->type == JSMN_OBJECT
		       || j->toks->type == JSMN_ARRAY);
		stest(j, &buffers[i]);
	}
}

static struct command_result *param_tok(struct command *cmd, const char *name,
					const char *buffer, const jsmntok_t * tok,
					const jsmntok_t **out)
{
	*out = tok;
	return NULL;
}

/*
 * Make sure toks are passed through correctly, and also make sure
 * optional missing toks are set to NULL.
 */
static void tok_tok(void)
{
	{
		unsigned int n;
		const jsmntok_t *tok = NULL;
		struct json *j = json_parse(cmd, "{ 'satoshi': '546' }");

		assert(param(cmd, j->buffer, j->toks,
			     p_req("satoshi", param_tok, &tok), NULL));
		assert(tok);
		assert(json_to_number(j->buffer, tok, &n));
		assert(n == 546);
	}
	// again with missing optional parameter
	{
		/* make sure it is *not* NULL */
		const jsmntok_t *tok = (const jsmntok_t *) 65535;

		struct json *j = json_parse(cmd, "{}");
		assert(param(cmd, j->buffer, j->toks,
			     p_opt("satoshi", param_tok, &tok), NULL));

		/* make sure it *is* NULL */
		assert(tok == NULL);
	}
}

/* check for valid but duplicate json name-value pairs */
static void dup_names(void)
{
	struct json *j =
		json_parse(cmd,
			   "{ 'u64' : '42', 'u64' : '43', 'fp' : '3.15' }");

	u64 *i;
	u64 *fp;
	assert(!param(cmd, j->buffer, j->toks,
		      p_req("u64", param_u64, &i),
		      p_req("fp", param_millionths, &fp), NULL));
}

static void null_params(void)
{
	uint64_t **intptrs = tal_arr(cmd, uint64_t *, 7);
	/* no null params */
	struct json *j =
	    json_parse(cmd, "[ '10', '11', '12', '13', '14', '15', '16']");

	assert(param(cmd, j->buffer, j->toks,
		     p_req("0", param_u64, &intptrs[0]),
		     p_req("1", param_u64, &intptrs[1]),
		     p_req("2", param_u64, &intptrs[2]),
		     p_req("3", param_u64, &intptrs[3]),
		     p_opt_def("4", param_u64, &intptrs[4], 999),
		     p_opt("5", param_u64, &intptrs[5]),
		     p_opt("6", param_u64, &intptrs[6]),
		     NULL));
	for (int i = 0; i < tal_count(intptrs); ++i) {
		assert(intptrs[i]);
		assert(*intptrs[i] == i + 10);
	}

	/* missing at end */
	j = json_parse(cmd, "[ '10', '11', '12', '13', '14']");
	assert(param(cmd, j->buffer, j->toks,
		     p_req("0", param_u64, &intptrs[0]),
		     p_req("1", param_u64, &intptrs[1]),
		     p_req("2", param_u64, &intptrs[2]),
		     p_req("3", param_u64, &intptrs[3]),
		     p_opt("4", param_u64, &intptrs[4]),
		     p_opt("5", param_u64, &intptrs[5]),
		     p_opt_def("6", param_u64, &intptrs[6], 888),
		     NULL));
	assert(*intptrs[0] == 10);
	assert(*intptrs[1] == 11);
	assert(*intptrs[2] == 12);
	assert(*intptrs[3] == 13);
	assert(*intptrs[4] == 14);
	assert(!intptrs[5]);
	assert(*intptrs[6] == 888);
}

static void no_params(void)
{
	struct json *j = json_parse(cmd, "[]");
	assert(param(cmd, j->buffer, j->toks, NULL));

	j = json_parse(cmd, "[ 'unexpected' ]");
	assert(!param(cmd, j->buffer, j->toks, NULL));
}


#if DEVELOPER
/*
 * Check to make sure there are no programming mistakes.
 */
static void bad_programmer(void)
{
	u64 *ival;
	u64 *ival2;
	u64 *fpval;
	struct json *j = json_parse(cmd, "[ '25', '546', '26' ]");

	/* check for repeated names */
	assert(!param(cmd, j->buffer, j->toks,
		      p_req("repeat", param_u64, &ival),
		      p_req("fp", param_millionths, &fpval),
		      p_req("repeat", param_u64, &ival2), NULL));
	assert(check_fail());
	assert(strstr(fail_msg, "developer error"));

	assert(!param(cmd, j->buffer, j->toks,
		      p_req("repeat", param_u64, &ival),
		      p_req("fp", param_millionths, &fpval),
		      p_req("repeat", param_u64, &ival), NULL));
	assert(check_fail());
	assert(strstr(fail_msg, "developer error"));

	assert(!param(cmd, j->buffer, j->toks,
		      p_req("u64", param_u64, &ival),
		      p_req("repeat", param_millionths, &fpval),
		      p_req("repeat", param_millionths, &fpval), NULL));
	assert(check_fail());
	assert(strstr(fail_msg, "developer error"));

	/* check for repeated arguments */
	assert(!param(cmd, j->buffer, j->toks,
		      p_req("u64", param_u64, &ival),
		      p_req("repeated-arg", param_u64, &ival), NULL));
	assert(check_fail());
	assert(strstr(fail_msg, "developer error"));

	assert(!param(cmd, j->buffer, j->toks,
		      p_req("u64", (param_cbx) NULL, NULL), NULL));
	assert(check_fail());
	assert(strstr(fail_msg, "developer error"));

	/* Add required param after optional */
	j = json_parse(cmd, "[ '25', '546', '26', '1.1' ]");
	unsigned int *msatoshi;
	u64 *riskfactor_millionths;
	assert(!param(
	    cmd, j->buffer, j->toks, p_req("u64", param_u64, &ival),
	    p_req("fp", param_millionths, &fpval),
	    p_opt_def("msatoshi", param_number, &msatoshi, 100),
	    p_req("riskfactor", param_millionths, &riskfactor_millionths),
	    NULL));
	assert(*msatoshi);
	assert(*msatoshi == 100);
	assert(check_fail());
	assert(strstr(fail_msg, "developer error"));
}
#endif

static void add_members(struct param **params,
			char **obj,
			char **arr, unsigned int **ints)
{
	for (int i = 0; i < tal_count(ints); ++i) {
		const char *name = tal_fmt(*params, "%i", i);
		if (i != 0) {
			tal_append_fmt(obj, ", ");
			tal_append_fmt(arr, ", ");
		}
		tal_append_fmt(obj, "\"%i\" : %i", i, i);
		tal_append_fmt(arr, "%i", i);
		param_add(params, name, true,
			  typesafe_cb_preargs(struct command_result *, void **,
					      param_number,
					      &ints[i],
					      struct command *,
					      const char *,
					      const char *,
					      const jsmntok_t *),
			  &ints[i]);
	}
}

/*
 * A roundabout way of initializing an array of ints to:
 * ints[0] = 0, ints[1] = 1, ... ints[499] = 499
 */
static void five_hundred_params(void)
{
	struct param *params = tal_arr(NULL, struct param, 0);

	unsigned int **ints = tal_arr(params, unsigned int*, 500);
	char *obj = tal_fmt(params, "{ ");
	char *arr = tal_fmt(params, "[ ");
	add_members(&params, &obj, &arr, ints);
	tal_append_fmt(&obj, "}");
	tal_append_fmt(&arr, "]");

	/* first test object version */
	struct json *j = json_parse(params, obj);
	assert(param_arr(cmd, j->buffer, j->toks, params, false) == NULL);
	for (int i = 0; i < tal_count(ints); ++i) {
		assert(ints[i]);
		assert(*ints[i] == i);
		*ints[i] = 65535;
	}

	/* now test array */
	j = json_parse(params, arr);
	assert(param_arr(cmd, j->buffer, j->toks, params, false) == NULL);
	for (int i = 0; i < tal_count(ints); ++i) {
		assert(*ints[i] == i);
	}

	tal_free(params);
}

static void sendpay(void)
{
	struct json *j = json_parse(cmd, "[ 'A', '123', 'hello there' '547']");

	const jsmntok_t *routetok, *note;
	u64 *msatoshi;
	unsigned *cltv;

	if (!param(cmd, j->buffer, j->toks,
		   p_req("route", param_tok, &routetok),
		   p_req("cltv", param_number, &cltv),
		   p_opt("note", param_tok, &note),
		   p_opt("msatoshi", param_u64, &msatoshi),
		   NULL))
		assert(false);

	assert(note);
	assert(!strncmp("hello there", j->buffer + note->start,
			note->end - note->start));
	assert(msatoshi);
	assert(*msatoshi == 547);
}

static void deprecated_rename(void)
{
	struct json *j = json_parse(cmd, "{ 'u64': 42 }");
	u64 *u64;

	assert(param(cmd, j->buffer, j->toks,
		     p_req("u64|old_u64", param_u64, &u64),
		     NULL));
	assert(*u64 == 42);

	cmd->deprecated_apis = true;
	j = json_parse(cmd, "{ 'old_u64': 42 }");
	assert(param(cmd, j->buffer, j->toks,
		     p_req("u64|old_u64", param_u64, &u64),
		     NULL));
	cmd->deprecated_apis = false;
	assert(!param(cmd, j->buffer, j->toks,
		      p_req("u64|old_u64", param_u64, &u64),
		      NULL));
}

static void invalid_bech32m(void)
{
	int wit_version;
	uint8_t data_out[500];
	size_t data_out_len;

	/* Taken from BIP-350 */

	/* Correct */
	assert(segwit_addr_decode(&wit_version, data_out, &data_out_len, "bc", "BC1SW50QGDZ25J"));
	assert(wit_version == 16);
	assert(data_out_len == 2);

	/* Correct encoding, but expecting the wrong hrp*/
	assert(!segwit_addr_decode(&wit_version, data_out, &data_out_len, "tb", "BC1SW50QGDZ25J"));

	/* BIP350-valid, but fake HRP so was put in "invalid" section of BIP */
	assert(segwit_addr_decode(&wit_version, data_out, &data_out_len, "tc", "tc1p0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vq5zuyut"));

	/* Incorrect for various reasons (including wrong checksum between bech32 <->bech32m */
	assert(!segwit_addr_decode(&wit_version, data_out, &data_out_len, "bc", "bc1p0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vqh2y7hd"));
	assert(!segwit_addr_decode(&wit_version, data_out, &data_out_len, "tb", "tb1z0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vqglt7rf"));
	assert(!segwit_addr_decode(&wit_version, data_out, &data_out_len, "bc", "BC1S0XLXVLHEMJA6C4DQV22UAPCTQUPFHLXM9H8Z3K2E72Q4K9HCZ7VQ54WELL"));
	assert(!segwit_addr_decode(&wit_version, data_out, &data_out_len, "bc", "bc1qw508d6qejxtdg4y5r3zarvary0c5xw7kemeawh"));
	assert(!segwit_addr_decode(&wit_version, data_out, &data_out_len, "tb", "tb1q0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vq24jc47"));
	assert(!segwit_addr_decode(&wit_version, data_out, &data_out_len, "bc", "bc1p38j9r5y49hruaue7wxjce0updqjuyyx0kh56v8s25huc6995vvpql3jow4"));
	assert(!segwit_addr_decode(&wit_version, data_out, &data_out_len, "bc", "BC130XLXVLHEMJA6C4DQV22UAPCTQUPFHLXM9H8Z3K2E72Q4K9HCZ7VQ7ZWS8R"));
	assert(!segwit_addr_decode(&wit_version, data_out, &data_out_len, "bc", "bc1pw5dgrnzv"));
	assert(!segwit_addr_decode(&wit_version, data_out, &data_out_len, "bc", "bc1p0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7v8n0nx0muaewav253zgeav"));
	assert(!segwit_addr_decode(&wit_version, data_out, &data_out_len, "bc", "BC1QR508D6QEJXTDG4Y5R3ZARVARYV98GJ9P"));
	assert(!segwit_addr_decode(&wit_version, data_out, &data_out_len, "tb", "tb1p0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vq47Zagq"));
	assert(!segwit_addr_decode(&wit_version, data_out, &data_out_len, "bc", "bc1p0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7v07qwwzcrf"));
	assert(!segwit_addr_decode(&wit_version, data_out, &data_out_len, "tb", "tb1p0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vpggkg4j"));
	assert(!segwit_addr_decode(&wit_version, data_out, &data_out_len, "bc", "bc1gmk9yu"));

}

static void sendpay_nulltok(void)
{
	struct json *j = json_parse(cmd, "[ 'A', '123']");

	const jsmntok_t *routetok, *note = (void *) 65535;
	u64 *msatoshi;
	unsigned *cltv;

	if (!param(cmd, j->buffer, j->toks,
		   p_req("route", param_tok, &routetok),
		   p_req("cltv", param_number, &cltv),
		   p_opt("note", param_tok, &note),
		   p_opt("msatoshi", param_u64, &msatoshi),
		   NULL))
		assert(false);

	assert(note == NULL);
	assert(msatoshi == NULL);
}

static void advanced(void)
{
	{
		struct json *j = json_parse(cmd, "[ 'lightning', 24, 'tok', 543 ]");

		struct json_escape *label;
		u64 *msat;
		u64 *msat_opt1, *msat_opt2;
		const jsmntok_t *tok;

		assert(param(cmd, j->buffer, j->toks,
			     p_req("description", param_label, &label),
			     p_req("msat", param_u64, &msat),
			     p_req("tok", param_tok, &tok),
			     p_opt("msat_opt1", param_u64, &msat_opt1),
			     p_opt("msat_opt2", param_u64, &msat_opt2),
			     NULL));
		assert(label != NULL);
		assert(streq(label->s, "lightning"));
		assert(*msat == 24);
		assert(tok);
		assert(msat_opt1);
		assert(*msat_opt1 == 543);
		assert(msat_opt2 == NULL);
	}
	{
		struct json *j = json_parse(cmd, "[ 3, 'foo' ]");
		struct json_escape *label, *foo;
		assert(param(cmd, j->buffer, j->toks,
			      p_req("label", param_label, &label),
			      p_opt("foo", param_label, &foo),
			      NULL));
		assert(streq(label->s, "3"));
		assert(streq(foo->s, "foo"));
	}
	{
		u64 *msat;
		u64 *msat2;
		struct json *j = json_parse(cmd, "[ 3 ]");
		assert(param(cmd, j->buffer, j->toks,
			      p_opt_def("msat", param_u64, &msat, 23),
			      p_opt_def("msat2", param_u64, &msat2, 53),
			      NULL));
		assert(*msat == 3);
		assert(msat2);
		assert(*msat2 == 53);
	}
}

static void advanced_fail(void)
{
	{
		struct json *j = json_parse(cmd, "[ 'anyx' ]");
		u64 *msat;
		assert(!param(cmd, j->buffer, j->toks,
			      p_req("msat", param_u64, &msat),
			      NULL));
		assert(check_fail());
		assert(strstr(fail_msg, "msat: should be an unsigned 64 bit integer: invalid token '\"anyx\"'"));
	}
}

#define test_cb(cb, T, json_, value, pass) \
{ \
	struct json *j = json_parse(cmd, json_); \
	T *v; \
	struct command_result *ret = cb(cmd, "name", j->buffer, j->toks + 1, &v); \
	assert((ret == NULL) == pass);					\
	if (ret == NULL) { \
		assert(v); \
		assert(*v == value); \
	} \
}

static void param_tests(void)
{
	test_cb(param_bool, bool, "[ true ]", true, true);
	test_cb(param_bool, bool, "[ false ]", false, true);
	test_cb(param_bool, bool, "[ tru ]", false, false);
	test_cb(param_bool, bool, "[ 1 ]", false, false);

	test_cb(param_millionths, u64, "[ -0.01 ]", 0, false);
	test_cb(param_millionths, u64, "[ 0.00 ]", 0, true);
	test_cb(param_millionths, u64, "[ 1 ]", 1000000, true);
	test_cb(param_millionths, u64, "[ 1.1 ]", 1100000, true);
	test_cb(param_millionths, u64, "[ 1.01 ]", 1010000, true);
	test_cb(param_millionths, u64, "[ 99.99 ]", 99990000, true);
	test_cb(param_millionths, u64, "[ 100.0 ]", 100000000, true);
	test_cb(param_millionths, u64, "[ 100.001 ]", 100001000, true);
	test_cb(param_millionths, u64, "[ 1000 ]", 1000000000, true);
	test_cb(param_millionths, u64, "[ 'wow' ]", 0, false);
}

static void test_invoice(struct command *cmd,
			 const char *buffer,
			 const jsmntok_t *obj UNNEEDED,
			 const jsmntok_t *params)
{
	u64 *msatoshi_val;
	struct json_escape *label_val;
	const char *desc_val;
	u64 *expiry;
	const jsmntok_t *fallbacks;
	const jsmntok_t *preimagetok;

	assert(cmd->mode == CMD_USAGE);
	if (!param(cmd, buffer, params,
		  p_req("msatoshi", param_u64, &msatoshi_val),
		  p_req("label", param_label, &label_val),
		  p_req("description", param_escaped_string, &desc_val),
		  p_opt("expiry", param_u64, &expiry),
		  p_opt("fallbacks", param_array, &fallbacks),
		  p_opt("preimage", param_tok, &preimagetok), NULL))
		return;

	/* should not be here since we are in the mode of CMD_USAGE
	 * and it always returns false. */
	abort();
}

static void usage(void)
{
	cmd->mode = CMD_USAGE;

	test_invoice(cmd, NULL, NULL, NULL);
	assert(streq(cmd->usage,
		     "msatoshi label description "
		     "[expiry] [fallbacks] [preimage]"));

	cmd->mode = CMD_NORMAL;
}

int main(int argc, char *argv[])
{
	common_setup(argv[0]);
	cmd = tal(tmpctx, struct command);
	cmd->mode = CMD_NORMAL;
	fail_msg = tal_arr(cmd, char, 10000);

	zero_params();
	sanity();
	tok_tok();
	null_params();
	no_params();
#if DEVELOPER
	bad_programmer();
#endif
	dup_names();
	five_hundred_params();
	sendpay();
	sendpay_nulltok();
	advanced();
	advanced_fail();
	param_tests();
	usage();
	deprecated_rename();
	invalid_bech32m();

	printf("run-params ok\n");
	common_shutdown();
}
