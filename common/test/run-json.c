#include "../json.c"
#include <ccan/mem/mem.h>
#include <common/utils.h>
#include <stdio.h>

/* AUTOGENERATED MOCKS START */
/* AUTOGENERATED MOCKS END */


// issue #577

static void do_json_tok_bitcoin_amount(const char* val, uint64_t expected)
{
	uint64_t amount;
	jsmntok_t tok;

	tok.start = 0;
	tok.end = strlen(val);

	fprintf(stderr, "do_json_tok_bitcoin_amount(\"%s\", %"PRIu64"): ", val, expected);

	assert(json_to_bitcoin_amount(val, &tok, &amount) == true);
	assert(amount == expected);

	fprintf(stderr, "ok\n");
}


static int test_json_tok_bitcoin_amount(void)
{
	do_json_tok_bitcoin_amount("0.00000001", 1);
	do_json_tok_bitcoin_amount("0.00000007", 7);
	do_json_tok_bitcoin_amount("0.00000008", 8);
	do_json_tok_bitcoin_amount("0.00000010", 10);
	do_json_tok_bitcoin_amount("0.12345678", 12345678);
	do_json_tok_bitcoin_amount("0.01234567", 1234567);
	do_json_tok_bitcoin_amount("123.45678900", 12345678900);

	return 0;
}

static void test_json_delve(void)
{
	const jsmntok_t *toks, *t;
	char *buf;
	bool ok;

	buf = "{\"1\":\"one\", \"2\":\"two\", \"3\":[\"three\", {\"deeper\": 17}]}";
	toks = json_parse_input(tmpctx, buf, strlen(buf), &ok);
	assert(ok);

	t = json_delve(buf, toks, ".1");
	assert(t);
	assert(t->type == JSMN_STRING);
	assert(json_tok_streq(buf, t, "one"));
	assert(t == toks+2);

	t = json_delve(buf, toks, ".2");
	assert(t);
	assert(t->type == JSMN_STRING);
	assert(json_tok_streq(buf, t, "two"));
	assert(t == toks+4);

	t = json_delve(buf, toks, ".3");
	assert(t);
	assert(t->type == JSMN_ARRAY);
	assert(t == toks+6);

	t = json_delve(buf, toks, ".3[0]");
	assert(t);
	assert(t->type == JSMN_STRING);
	assert(json_tok_streq(buf, t, "three"));
	assert(t == toks+7);

	t = json_delve(buf, toks, ".3[1]");
	assert(t);
	assert(t->type == JSMN_OBJECT);
	assert(t == toks+8);

	t = json_delve(buf, toks, ".3[1].deeper");
	assert(t);
	assert(t->type == JSMN_PRIMITIVE);
	assert(memeq(buf + t->start, t->end - t->start, "17", strlen("17")));
	assert(t == toks+10);

	t = json_delve(buf, toks, ".4");
	assert(!t);
	t = json_delve(buf, toks, "[0]");
	assert(!t);
	t = json_delve(buf, toks, ".deeper");
	assert(!t);
	t = json_delve(buf, toks, ".3[2]");
	assert(!t);
	t = json_delve(buf, toks, ".3[2].deeper");
	assert(!t);
	t = json_delve(buf, toks, ".3[0].deeper");
	assert(!t);
	t = json_delve(buf, toks, ".3[1].deeper[0]");
	assert(!t);
	t = json_delve(buf, toks, ".3[1][0]");
	assert(!t);

	/* Now a real example. */
	buf = "{\n"
		"  \"jsonrpc\": \"2.0\", \n"
		"  \"method\": \"init\", \n"
		"  \"id\": 1, \n"
		"  \"params\": {\n"
		"    \"options\": {\n"
		"    }, \n"
		"    \"configuration\": {\n"
		"      \"lightning-dir\": \"/tmp/ltests-n2hyd543/test_pay_plugin_1/lightning-2/\", \n"
		"      \"rpc-file\": \"lightning-rpc\"\n"
		"    }\n"
		"  }\n"
		"}\n"
		"\n";
	toks = json_parse_input(tmpctx, buf, strlen(buf), &ok);
	assert(ok);

	t = json_delve(buf, toks, ".rpcfile");
	assert(!t);
	t = json_delve(buf, toks, ".configuration.rpc-file");
	assert(!t);
	t = json_delve(buf, toks, ".params.configuration.rpc-file");
	assert(t);
	assert(t->type == JSMN_STRING);
	assert(json_tok_streq(buf, t, "lightning-rpc"));
}

int main(void)
{
	setup_locale();
	setup_tmpctx();

	test_json_tok_bitcoin_amount();
	test_json_delve();
	assert(!taken_any());
	take_cleanup();
	tal_free(tmpctx);
}
