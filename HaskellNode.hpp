#ifndef HASKELL_NODE_H
#define HASKELL_NODE_H

#include <tao/pegtl/contrib/parse_tree.hpp>


struct haskell_node : TAO_PEGTL_NAMESPACE::parse_tree::node {
    template< typename Rule, typename ParseInput, typename... States >
    void success( const ParseInput& in, States&&... st ) {
        std::cout << "Yay!!!\n";
        TAO_PEGTL_NAMESPACE::parse_tree::node::success<Rule, ParseInput, States>(in, st);
    }
};


#endif