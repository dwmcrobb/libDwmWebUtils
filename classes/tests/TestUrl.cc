#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include "DwmUnitAssert.hh"
#include "DwmWebUtilsUrl.hh"

using namespace std;

typedef struct {
  string    url;
  string    scheme;
  string    userinfo;
  string    host;
  uint16_t  port;
  string    path;
  string    query;
  string    fragment;
} TestData_t;

static TestData_t goodUrls[] = {
  { "http://foo.com/blah_blah",
    "http", "", "foo.com", 80, "/blah_blah", "", "" },
  { "http://foo.com/blah_blah/",
    "http", "", "foo.com", 80, "/blah_blah/", "", "" },
  { "http://foo.com/blah_blah_(wikipedia)",
    "http", "", "foo.com", 80, "/blah_blah_(wikipedia)", "", "" },
  { "http://foo.com/blah_blah_(wikipedia)_(again)",
    "http", "", "foo.com", 80, "/blah_blah_(wikipedia)_(again)", "", "" },
  { "http://www.example.com/wpstyle/?p=364",
    "http", "", "www.example.com", 80, "/wpstyle/", "p=364", "" },
  { "https://www.example.com/foo/?bar=baz&inga=42&quux",
    "https", "", "www.example.com", 443, "/foo/", "bar=baz&inga=42&quux", "" },
  { "http://userid:password@example.com:8080",
    "http", "userid:password", "example.com", 8080, "", "", "" },
  { "http://userid:password@example.com:8080/",
    "http", "userid:password", "example.com", 8080, "/", "", "" },
  { "http://userid@example.com",
    "http", "userid", "example.com", 80, "", "", "" },
  { "http://userid@example.com/",
    "http", "userid", "example.com", 80, "/", "", "" },
  { "http://userid@example.com:8080",
    "http", "userid", "example.com", 8080, "", "", "" },
  { "http://userid@example.com:8080/",
    "http", "userid", "example.com", 8080, "/", "", "" },
  { "http://userid:password@example.com",
    "http", "userid:password", "example.com", 80, "", "", "" },
  { "http://userid:password@example.com/",
    "http", "userid:password", "example.com", 80, "/", "", "" },
  { "http://142.42.1.1/",
    "http", "", "142.42.1.1", 80, "/", "", "" },
  { "http://142.42.1.1:8080/",
    "http", "", "142.42.1.1", 8080, "/", "", "" },
  { "http://foo.com/blah_(wikipedia)#cite-1",
    "http", "", "foo.com", 80, "/blah_(wikipedia)", "", "cite-1" },
  { "http://foo.com/blah_(wikipedia)_blah#cite-1",
    "http", "", "foo.com", 80, "/blah_(wikipedia)_blah", "", "cite-1" },
  { "http://foo.com/(something)?after=parens",
    "http", "", "foo.com", 80, "/(something)", "after=parens", "" },
  { "http://code.google.com/events/#&product=browser",
    "http", "", "code.google.com", 80, "/events/", "", "&product=browser" },
  { "http://j.mp",
    "http", "", "j.mp", 80, "", "", "" },
  { "ftp://foo.bar/baz",
    "ftp", "", "foo.bar", 21, "/baz", "", "" },
  { "http://foo.bar/?q=Test%20URL-encoded%20stuff",
    "http", "", "foo.bar", 80, "/", "q=Test%20URL-encoded%20stuff", "" },
  { "http://-.~_!$&'()*+,;=:%40:80%2f::::::@example.com",
    "http", "-.~_!$&'()*+,;=:%40:80%2f::::::", "example.com", 80, "", "", "" },
  { "http://1337.net",
    "http", "", "1337.net", 80, "", "", "" },
  { "http://a.b-c.de",
    "http", "", "a.b-c.de", 80, "", "", "" },
  { "http://223.255.255.254",
    "http", "", "223.255.255.254", 80, "", "", "" }
};

static const std::string  badUrls[] = {
  "http://", 
  "http://.",
  "http://..",
  "http://../",
  "http://?",
  "http://??",
  "http://?\?/",
  "http://#",
  "http://##",
  "http://##/",
  "http://foo.bar?q=Spaces should be encoded",
  "//",
  "//a",
  "///a",
  "///",
  "http:///a",
  "foo.com",
  "rdar://1234",
  "h://test",
  "http:// shouldfail.com",
  ":// should fail",
  "http://foo.bar/foo(bar)baz quux",
  "ftps://foo.bar/",
  "http://-error-.invalid/",
  "http://a.b--c.de/",
  "http://-a.b.co",
  "http://a.b-.co",
  "http://0.0.0.0",
  "http://224.1.1.1",
  "http://1.1.1.1.1",
  "http://123.123.123",
  "http://3628126748",
  "http://.www.foo.bar/",
  "http://.www.foo.bar./"
};

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Dwm::WebUtils::Url  webUrl;
  for (int i = 0; i < sizeof(goodUrls) / sizeof(goodUrls[0]); ++i) {
    UnitAssert(webUrl.Parse(goodUrls[i].url));
    UnitAssert(webUrl.Scheme() == goodUrls[i].scheme);
    if (! UnitAssert(webUrl.UserInfo() == goodUrls[i].userinfo)) {
      cerr << webUrl.UserInfo() << " != " << goodUrls[i].userinfo << '\n';
    }
    if (! UnitAssert(webUrl.Host() == goodUrls[i].host)) {
      cerr << webUrl.Host() << " != " << goodUrls[i].host << '\n';
    }
    if (! UnitAssert(webUrl.Port() == goodUrls[i].port)) {
      cerr << webUrl.Port() << " != " << goodUrls[i].port << '\n';
    }
    UnitAssert(webUrl.Path() == goodUrls[i].path);
    UnitAssert(webUrl.Query() == goodUrls[i].query);
    UnitAssert(webUrl.Fragment() == goodUrls[i].fragment);
  }

  for (int i = 0; i < sizeof(badUrls) / sizeof(badUrls[0]); ++i) {
    if (! UnitAssert(! webUrl.Parse(badUrls[i]))) {
      cerr << "failed on '" << badUrls[i] << "'\n";
    }
  }
      
  if (Dwm::Assertions::Total().Failed())
    Dwm::Assertions::Print(cerr, true);
  else
    cout << Dwm::Assertions::Total() << " passed" << endl;

}
