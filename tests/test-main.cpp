# define CATCH_CONFIG_MAIN
# include "catch.hpp"

TEST_CASE("Tests that Catch2 is properly configured", "[meta]")
{
  REQUIRE(1==1);
}