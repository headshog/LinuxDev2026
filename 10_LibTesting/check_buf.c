/* Based on tests.c from skeeto/growable-buf; public domain, see UNLICENSE. */
#include <check.h>
#include "buf.h"

static float *values;
static long *integers;

static void setup(void)
{
    values = NULL;
    integers = NULL;
}

static void teardown(void)
{
    buf_free(values);
    buf_free(integers);
}

START_TEST(test_empty)
{
    ck_assert_uint_eq(buf_capacity(values), 0);
    ck_assert_uint_eq(buf_size(values), 0);
    buf_clear(values);
    ck_assert_uint_eq(buf_size(values), 0);
    ck_assert_ptr_null(values);
}
END_TEST

START_TEST(test_first_push)
{
    buf_push(values, 1.3f);
    ck_assert_uint_eq(buf_size(values), 1);
    ck_assert_uint_eq(buf_capacity(values), BUF_INIT_CAPACITY);
    ck_assert_float_eq(values[0], 1.3f);
}
END_TEST

START_TEST(test_clear)
{
    buf_push(values, 1.3f);
    size_t capacity = buf_capacity(values);
    buf_clear(values);
    ck_assert_uint_eq(buf_size(values), 0);
    ck_assert_uint_eq(buf_capacity(values), capacity);
    ck_assert_ptr_nonnull(values);
    buf_push(values, 2.5f);
    ck_assert_uint_eq(buf_size(values), 1);
    ck_assert_float_eq(values[0], 2.5f);
}
END_TEST

START_TEST(test_free)
{
    buf_push(values, 1.3f);
    buf_free(values);
    ck_assert_ptr_null(values);
    buf_free(values);
    ck_assert_ptr_null(values);
    ck_assert_uint_eq(buf_size(values), 0);
    ck_assert_uint_eq(buf_capacity(values), 0);
}
END_TEST

START_TEST(test_push_many)
{
    for (int i = 0; i < 10000; ++i)
        buf_push(integers, i);
    ck_assert_uint_eq(buf_size(integers), 10000);
    int match = 0;
    for (size_t i = 0; i < buf_size(integers); ++i)
        match += integers[i] == (long)i;
    ck_assert_int_eq(match, 10000);
}
END_TEST

START_TEST(test_grow_and_trunc)
{
    buf_grow(integers, 1000);
    ck_assert_uint_eq(buf_capacity(integers), 1000);
    ck_assert_uint_eq(buf_size(integers), 0);
    buf_trunc(integers, 100);
    ck_assert_uint_eq(buf_capacity(integers), 100);
    ck_assert_uint_eq(buf_size(integers), 0);
    buf_grow(integers, 0);
    ck_assert_uint_eq(buf_capacity(integers), 100);
}
END_TEST

START_TEST(test_pop)
{
    buf_push(values, 1.1f);
    buf_push(values, 1.2f);
    buf_push(values, 1.3f);
    buf_push(values, 1.4f);
    ck_assert_uint_eq(buf_size(values), 4);
    ck_assert_float_eq(buf_pop(values), 1.4f);
    buf_trunc(values, 3);
    ck_assert_uint_eq(buf_size(values), 3);
    ck_assert_float_eq(buf_pop(values), 1.3f);
    ck_assert_float_eq(buf_pop(values), 1.2f);
    ck_assert_float_eq(buf_pop(values), 1.1f);
    ck_assert_uint_eq(buf_size(values), 0);
}
END_TEST

START_TEST(test_trunc_clamps_size)
{
    for (long i = 0; i < 4; ++i)
        buf_push(integers, i + 10);
    buf_trunc(integers, 2);
    ck_assert_uint_eq(buf_capacity(integers), 2);
    ck_assert_uint_eq(buf_size(integers), 2);
    ck_assert_int_eq(integers[0], 10);
    ck_assert_int_eq(integers[1], 11);
    buf_push(integers, 99);
    ck_assert_uint_eq(buf_size(integers), 3);
    ck_assert_int_eq(integers[2], 99);
}
END_TEST

START_TEST(test_zero_capacity)
{
    buf_grow(integers, 0);
    ck_assert_ptr_nonnull(integers);
    ck_assert_uint_eq(buf_capacity(integers), 0);
    buf_push(integers, 7);
    ck_assert_uint_eq(buf_size(integers), 1);
    ck_assert_int_eq(integers[0], 7);
    buf_trunc(integers, 0);
    ck_assert_uint_eq(buf_capacity(integers), 0);
    ck_assert_uint_eq(buf_size(integers), 0);
}
END_TEST

int main(void)
{
    Suite *suite = suite_create("growable-buf");
    TCase *state = tcase_create("state");
    TCase *growth = tcase_create("growth");
    TCase *stack = tcase_create("stack");

    tcase_add_checked_fixture(state, setup, teardown);
    tcase_add_checked_fixture(growth, setup, teardown);
    tcase_add_checked_fixture(stack, setup, teardown);
    tcase_add_test(state, test_empty);
    tcase_add_test(state, test_first_push);
    tcase_add_test(state, test_clear);
    tcase_add_test(state, test_free);
    tcase_add_test(growth, test_push_many);
    tcase_add_test(growth, test_grow_and_trunc);
    tcase_add_test(growth, test_trunc_clamps_size);
    tcase_add_test(growth, test_zero_capacity);
    tcase_add_test(stack, test_pop);
    suite_add_tcase(suite, state);
    suite_add_tcase(suite, growth);
    suite_add_tcase(suite, stack);

    SRunner *runner = srunner_create(suite);
    /* A single process also ensures that gcov saves all counters on exit. */
    srunner_set_fork_status(runner, CK_NOFORK);
    srunner_run_all(runner, CK_VERBOSE);
    int failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
