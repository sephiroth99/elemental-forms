/**
 ******************************************************************************
 * Elemental Forms : a lightweight user interface framework                   *
 ******************************************************************************
 * Copyright 2015 Ben Vanik. All rights reserved. Licensed as BSD 3-clause.   *
 * Portions ©2011-2015 Emil Segerås: https://github.com/fruxo/turbobadger     *
 ******************************************************************************
 */

#ifndef EL_PARSING_PARSE_NODE_TREE_H_
#define EL_PARSING_PARSE_NODE_TREE_H_

#include <string>

#include "el/id.h"
#include "el/parsing/parse_node.h"
#include "el/util/intrusive_list.h"

namespace el {
namespace parsing {

class ParseNode;
class ParseNodeTreeListener;

// A named ParseNode.
// ParseNodes under this node may be referenced from other nodes, either when
// requesting a value (ParseNode::GetValueFollowRef), or while parsing the node
// tree.
// While parsing, the values can be used for branch conditions or branches of
// nodes can be included.
class ParseNodeTree : public util::IntrusiveListEntry<ParseNodeTree> {
 public:
  explicit ParseNodeTree(const char* name);
  virtual ~ParseNodeTree();

  const std::string& name() const { return m_name; }
  const TBID& name_id() const { return m_name_id; }

  // Reads the data file. This will *not* invoke any change listener!
  bool ReadFile(const char* filename) { return m_node.ReadFile(filename); }
  void ReadData(const char* data) { m_node.ReadData(data); }

  // Adds a listener that is invoked on changes in this tree.
  void AddListener(ParseNodeTreeListener* listener) {
    m_listeners.AddLast(listener);
  }

  // Removes a change listener from this tree.
  void RemoveListener(ParseNodeTreeListener* listener) {
    m_listeners.Remove(listener);
  }

  // Sets the value for the given request and invoke the change listener.
  // Creates the nodes that doesn't exist.
  virtual void SetValue(const char* request, const Value& value);

  // Gets the value of the given request. Follows references if any.
  // Returns a null value if the request doesn't exist.
  virtual Value& GetValue(const char* request);

  // Gets the value of the given tree name and request (@treename>noderequest).
  // Returns a null value if the given tree or request doesn't exist.
  static Value& GetValueFromTree(const char* request);

  // Returns the tree with the given name, or nullptr if no matching tree
  // exists.
  static ParseNodeTree* GetRefTree(const char* name, size_t name_len);

  // Goes through the tree of nodes recursively and include or remove branches
  // depending on any conditions.
  static void ResolveConditions(ParseNode* parent_node);

 private:
  friend class ParseNode;
  friend class ParseNodeTarget;

  // Follows any references to data trees and return the destination node.
  // If there's broken references, the node will be returned.
  static ParseNode* FollowNodeRef(ParseNode* node);

  void InvokeChangeListenersInternal(const char* request);

  ParseNode m_node;
  std::string m_name;
  TBID m_name_id;
  util::IntrusiveList<ParseNodeTreeListener> m_listeners;
  static util::IntrusiveList<ParseNodeTree> s_ref_trees;
};

// Receives OnDataChanged when the value of a node in a ParseNodeTree is
// changed.
// FIX: The listener can currently only listen to one tree.
class ParseNodeTreeListener
    : public util::IntrusiveListEntry<ParseNodeTreeListener> {
 public:
  // Called when the value is changed for the given node in the given ref tree.
  // The request is without tree name.
  virtual void OnDataChanged(ParseNodeTree* rt, const char* request) = 0;
};

}  // namespace parsing
}  // namespace el

#endif  // EL_PARSING_PARSE_NODE_TREE_H_
