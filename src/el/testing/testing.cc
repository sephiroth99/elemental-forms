/**
 ******************************************************************************
 * Elemental Forms : a lightweight user interface framework                   *
 ******************************************************************************
 * Copyright 2015 Ben Vanik. All rights reserved. Licensed as BSD 3-clause.   *
 * Portions ©2011-2015 Emil Segerås: https://github.com/fruxo/turbobadger     *
 ******************************************************************************
 */

#include <string>

#include "el/io/file_manager.h"
#include "el/io/posix_file_system.h"
#include "el/testing/testing.h"
#include "el/util/debug.h"

#ifdef EL_UNIT_TESTING
// Reference at least one group in each test file, to force
// linking the object file. This is needed if TB is compiled
// as an library.
EL_FORCE_LINK_TEST_GROUP(tb_color);
EL_FORCE_LINK_TEST_GROUP(tb_dimension_converter);
EL_FORCE_LINK_TEST_GROUP(tb_geometry);
EL_FORCE_LINK_TEST_GROUP(tb_linklist);
EL_FORCE_LINK_TEST_GROUP(tb_node_ref_tree);
EL_FORCE_LINK_TEST_GROUP(tb_object);
EL_FORCE_LINK_TEST_GROUP(tb_parser);
EL_FORCE_LINK_TEST_GROUP(tb_space_allocator);
EL_FORCE_LINK_TEST_GROUP(tb_text_box);
EL_FORCE_LINK_TEST_GROUP(tb_string_builder);
EL_FORCE_LINK_TEST_GROUP(tb_test);
EL_FORCE_LINK_TEST_GROUP(tb_value);
EL_FORCE_LINK_TEST_GROUP(tb_widget_value_text);
#endif

namespace el {

#ifdef EL_UNIT_TESTING

uint32_t test_settings;
int fail_line_nr;
const char* fail_file;
const char* fail_text;

// We can't use a linked list object since we don't know if its constructor
// would run before of after any test group constructor that add itself
// to it. Using a manual one way link list is very simple.
TBTestGroup* g_test_groups = nullptr;

std::string tb_get_test_file_name(const char* testpath, const char* filename) {
  size_t test_path_len = std::strlen(testpath);
  for (size_t i = test_path_len - 1;
       i > 0 && testpath[i] != '/' && testpath[i] != '\\'; i--) {
    test_path_len = i;
  }
  std::string str(testpath, test_path_len);
  str.append(filename);
  return str;
}

TBRegisterCall::TBRegisterCall(TBTestGroup* test, TBCall* call) : call(call) {
  if (strcmp(call->name(), "Setup") == 0)
    test->setup = call;
  else if (strcmp(call->name(), "Cleanup") == 0)
    test->cleanup = call;
  else if (strcmp(call->name(), "Init") == 0)
    test->init = call;
  else if (strcmp(call->name(), "Shutdown") == 0)
    test->shutdown = call;
  else
    test->calls.AddLast(call);
}

TBRegisterCall::~TBRegisterCall() {
  if (call->linklist) call->linklist->Remove(call);
}

TBTestGroup::TBTestGroup(const char* name)
    : name(name),
      setup(nullptr),
      cleanup(nullptr),
      init(nullptr),
      shutdown(nullptr),
      next_test_group(g_test_groups) {
  g_test_groups = this;
}

const char* CallAndOutput(TBTestGroup* test, TBCall* call) {
  fail_text = nullptr;
  call->exec();

  if (!fail_text) return fail_text;
  TBDebugOut(
      "FAIL: \"%s/%s\":\n"
      "  %s(%d): \"%s\"\n",
      test->name, call->name(), fail_file, fail_line_nr, fail_text);
  return fail_text;
}

void OutputPass(TBTestGroup* test, const char* call_name) {
  if (!(test_settings & EL_TEST_VERBOSE)) return;
  TBDebugOut("PASS: \"%s/%s\"\n", test->name, call_name);
}

int TBRunTests(uint32_t settings) {
  test_settings = settings;
  int num_failed = 0;
  int num_passed = 0;

  TBDebugOut("Running tests...\n");

  el::io::FileManager::RegisterFileSystem(
      std::make_unique<el::io::PosixFileSystem>("."));

  for (TBTestGroup* group = g_test_groups; group;
       group = group->next_test_group) {
    if (group->init && CallAndOutput(group, group->init)) {
      // The whole group failed because init failed.
      int num_tests_in_group = 0;
      for (TBCall* call = group->calls.GetFirst(); call; call = call->GetNext())
        if (!group->IsSpecialTest(call)) num_tests_in_group++;

      TBDebugOut("  %d tests skipped.\n", num_tests_in_group);

      num_failed += num_tests_in_group;
      continue;
    }
    for (TBCall* call = group->calls.GetFirst(); call; call = call->GetNext()) {
      // Execute test (and call setup and cleanup if available).
      int fail = 0;
      if (group->setup) fail = !!CallAndOutput(group, group->setup);
      if (!fail) {  // Only run if setup succeeded
        fail |= !!CallAndOutput(group, call);
        if (group->cleanup) fail |= !!CallAndOutput(group, group->cleanup);
      }
      // Handle result
      if (fail) {
        num_failed++;
      } else {
        num_passed++;
        OutputPass(group, call->name());
      }
    }
    if (group->shutdown && CallAndOutput(group, group->shutdown))
      CallAndOutput(group, group->shutdown);
  }

  TBDebugOut("Test results: %d passed, %d failed.\n", num_passed, num_failed);
  return num_failed;
}

#endif  // EL_UNIT_TESTING

}  // namespace el
