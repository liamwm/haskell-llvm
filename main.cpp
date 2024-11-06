#include <iostream>
#include <memory>
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/trace.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>

#include "HaskellNode.hpp"

using namespace std;
using namespace TAO_PEGTL_NAMESPACE;


namespace lexeme
{
  struct varid : seq< lower, star< alpha > > {};
  struct conid : seq< upper, star< alpha > > {};

  struct symbol : sor<
    one< '!' >,
    one< '#' >,
    one< '$' >,
    one< '%' >,
    one< '&' >,
    one< '*' >,
    one< '+' >,
    one< '.' >,
    one< '/' >,
    one< '<' >,
    one< '=' >,
    one< '>' >,
    one< '?' >,
    one< '@' >,
    one< '\\' >,
    one< '^' >,
    one< '|' >,
    one< '-' >,
    one< '~' >
  > {};

  struct varsym : seq<symbol, star< sor<symbol, one< ':' > > > > {};
  struct consym : seq< one< ':' >, star< sor<symbol, one< ':' > > > > {};

  struct l_integer : seq< digit, star< digit > > {};
  struct l_string : seq< one< '"' >, star< sor< alpha, one< ' ' > > >, one< '"' > > {};
  struct literal : sor< l_integer, l_string > {};

  struct special_semicolon : one<';'> {};
  struct special : sor<
    one< '(' >,
    one< ')' >,
    one< ',' >,
    special_semicolon,
    one< '[' >,
    one< ']' >,
    one< '`' >,
    one< '{' >,
    one< '}' >
  > {};

  struct reservedop_type_sig : tao::pegtl::string< ':', ':' > {};
  struct reservedop : sor<
    tao::pegtl::string< '.', '.' >,
    reservedop_type_sig,
    tao::pegtl::string< ':' >,
    tao::pegtl::string< '=' >,
    tao::pegtl::string< '\\' >,
    tao::pegtl::string< '|' >,
    tao::pegtl::string< '<', '-' >,
    tao::pegtl::string< '-', '>' >,
    tao::pegtl::string< '@' >,
    tao::pegtl::string< '~' >,
    tao::pegtl::string< '=', '>' >
  > {};

  struct lexeme : sor< varid, conid, reservedop, varsym, consym, literal, special > {};

  struct whitespace : tao::pegtl::plus< sor< eol, blank > > {};

  struct program : seq< star< sor< lexeme, whitespace > >, eof > {};
}

// template<> struct selector< lexeme::varid > : std::true_type {};
// template<> struct selector< lexeme::conid > : std::true_type {};
// template<> struct selector< lexeme::varsym > : std::true_type {};
// template<> struct selector< lexeme::consym > : std::true_type {};
// template<> struct selector< lexeme::literal > : std::true_type {};

namespace expression {
  // struct atype : lexeme::conid {};
  // struct btype : seq< star< btype >, atype > {};
  // struct type_expr : seq< btype, star< seq< tao::pegtl::string< '-', '>' >, type_expr > > > {};
  // struct type_sig : seq< lexeme::varid, lexeme::reservedop_type_sig, type_expr > {};
  struct infixexp;
  struct infix_op_application;
  struct var : lexeme::varid {};
  struct varop : lexeme::varsym {};
  struct conop : lexeme::consym {};
  struct aexp : sor< var, lexeme::literal > {};
  struct fexp : seq< aexp, star< seq< one<' '>, fexp > > > {};
  struct lexp : sor< fexp > {};
  struct qop : sor< varop, conop > {};
  struct infix_op_application : seq< lexp, qop, infixexp > {};
  struct infixexp : sor< infix_op_application, lexp > {};
  struct exp : infixexp {};
}

// template<> struct selector< expression::lexp > : std::true_type {};
// template<> struct selector< expression::infix_op_application > : std::true_type {};

namespace pattern {
  struct pat;
  struct lpat;
  struct apat;

  struct apat : sor< expression::var, lexeme::literal > {};
  struct lpat : sor< apat > {};
  struct infix_constructor : seq< lpat, expression::conop, pat > {};
  struct pat : sor< infix_constructor, lpat > {};
}

namespace declaration {
  struct atype : sor< lexeme::conid > {};
  struct btype : seq< atype, star< btype > > {};
  struct type_expr : seq< btype, opt< seq< tao::pegtl::string< '-', '>' >, type_expr > > > {};
  struct type_sig : seq< lexeme::varid, lexeme::reservedop_type_sig, type_expr > {};
  // 4.4.3 function bindings
  struct funlhs : seq< expression::var, tao::pegtl::plus< seq< one<' '>, pattern::apat > > > {};
  struct rhs : seq< tao::pegtl::string< '=' >, expression::exp > {};
  struct decl : sor< type_sig, seq< sor< funlhs, pattern::pat >, rhs > > {};
  struct body : seq< star< seq< decl, lexeme::special_semicolon> >, decl > {};
}

// template<> struct selector< declaration::decl > : std::true_type {};
// template<> struct selector< declaration::rhs > : std::true_type {};
// template<> struct selector< declaration::funlhs > : std::true_type {};
// template<> struct selector< declaration::type_sig > : std::true_type {};
// template<> struct selector< declaration::type_expr > : std::true_type {};
// template<> struct selector< declaration::atype > : std::true_type {};
// template<> struct selector< declaration::btype > : std::true_type {};

template< typename Rule > struct selector : std::true_type {};
template<> struct selector< alpha > : std::false_type {};
template<> struct selector< digit > : std::false_type {};
template<> struct selector< lexeme::symbol > : std::false_type {};
template<> struct selector< lexeme::special > : std::false_type {};
template<> struct selector< lexeme::special_semicolon > : std::false_type {};


int main(int argc, char *argv[]) {
  char* filepath = argv[1];

  // Parser parser(filepath);
  // parser.parse();

  file_input in(filepath);
  //parse< lexeme::program, lexeme::action >(in);
  // complete_trace< declaration::body >( in );
  auto root = parse_tree::parse< declaration::body, haskell_node >(in);
  parse_tree::print_dot(cout, *root);

  return 0;
}