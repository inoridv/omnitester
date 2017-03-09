"use strict";
var page = require('webpage').create();
var server = require('webserver').create();
var system = require('system');
var host, port;


// TODO: test this.
function sleep(milliseconds) {
  var start = new Date().getTime();
  for (var i = 0; i < 1e7; i++) {
    if ((new Date().getTime() - start) > milliseconds){
      break;
    }
  }
}

function responseCrafter(response, testResultData) {
  // TODO: Check if response is passed by reference.

  // TODO: issets.
  response.statusCode = 200;
  response.headers = {"Cache": "no-cache", "Content-Type": "text/html"};

  // This is also possible.
  //  response.setHeader("foo", "bar");

  response.write(JSON.stringify(testResultData));

  response.close();
}

if (system.args.length < 2) {
    console.log('Usage: server.js <some port>');
    phantom.exit(1);
} else {
    port = system.args[1];
    var listening = server.listen(port, function (request, response) {
        console.log("Received HTTP Request");
        var jsonRequest = JSON.stringify(request, null, 4);
        var testData = JSON.parse(jsonRequest);
        console.log(JSON.stringify(testData));

        // Runtest Start
        var page = require('webpage').create();
        var system = require('system');
        var regex = new RegExp("([^?=&]+)(=([^&]*))?", "g");
        var testResult = {};
        // TODO: as this isnt working, try to store each request on a var and work with a foreach near the creation of the rsponse text, and then
        // if no request of the type is found we insert the message :D.
        var linkRequest = 0;

        console.log("Starting Test \n");
        console.log(JSON.stringify(testData));

        page.onResourceRequested = function (req) {
            var requeststring = JSON.stringify(req, undefined, 4);
            var jsonrequest = JSON.parse(requeststring);
            url = jsonrequest.url;
            // TODO: Fill response with something if no requests matching the
            // identifier are found.
            if (jsonrequest.url.indexOf(testData.post.RequestIdentifier) !== -1) {
              var queryString = {};
              url = decodeURIComponent(url);
              // TODO: Check and improve this code if possible.
              url.replace(regex, function($0, $1, $2, $3) { queryString[$1] = $3; });
              if (!testResult.Type) {
                testResult.Type = {};
              }
              console.log(JSON.stringify(queryString));
              console.log("TESTRESULT CHECK\n")
              console.log(JSON.stringify(testResult));
              testResult.Type = testData.post.Type;
              if (testData.post.Type == 'Link') {
                if (queryString.link) {
                  console.log("Running Link Type Test" + "\n");
                  if (!testResult.LinkType) {
                    testResult.LinkType = {};
                  }
                  if (testData.post.LinkType == queryString.pe) {
                    testResult.LinkType.status = 'Pass';
                  }
                  else {
                    testResult.LinkType.status = 'Fail';
                  }
                  testResult.LinkType.found = queryString.pe;
                  testResult.LinkType.expected = testData.post.LinkType;
                  console.log("Link Type Test Finished");

                  console.log("Running Link Text Test" + "\n");
                  if (!testResult.Link) {
                    testResult.Link = {};
                  }
                  if (testData.post.Link == queryString.link) {
                    testResult.Link.status = 'Pass';
                  }
                  else {
                    testResult.Link.status = 'Fail';
                  }
                  if (!testResult.Link.found) {
                    testResult.Link.found = queryString.link;
                  }
                  testResult.Link.expected = testData.post.Link;
                  console.log("Link Text Test Finished");
                }
                else {
                  if (linkRequest == 1) {
                    testResult.executed = 'Fail';
                    testResult.message = 'Unable to find a request that matches the test type';
                  }
                }
              }
              else {
                console.log("Running Page Name Test" + "\n");
                if (!queryString.link) {
                  if (!testResult.PageName) {
                    testResult.PageName = {};
                  }
                  if (testData.post.PageName == queryString.pageName) {
                    testResult.PageName.status = 'Pass';
                  }
                  else {
                    testResult.PageName.status = 'Fail';
                  }
                  testResult.PageName.found = queryString.pageName;
                  testResult.PageName.expected = testData.post.PageName;
                  console.log("Page Name Test Finished");
                }
                else {
                  if (linkRequest == 1) {
                    testResult.executed = 'Fail';
                    testResult.message = 'Unable to find a request that matches the test type';
                  }
                }
              }
            }
        };

        page.onConsoleMessage = function (msg) { console.log("pagelogee  " + msg + "\n"); };

        page.settings.userName = testData.post.username;
        page.settings.password = testData.post.password;
        var pageUrl = testData.post.base_url + testData.post.RelativePath;

        page.open(pageUrl, function (status) {
            if (status === "success") {
              setTimeout(function(linkRequest, testResult){
                if(testData.post.Element && testData.post.Type == "Link") {
                  page.evaluate(function(testData, linkRequest, testResult) {
                    linkRequest = 1;
                    var ev = document.createEvent("MouseEvents");
                    ev.initEvent("click", true, true);
                    var element = document.querySelector(testData.post.Element);
                    if (element == null) {
                      if (!testResult.Link) {
                        testResult.Link = {};
                      }
                      testResult.Link.found = 'Element not found in the page';
                    }
                    else {
                      element.dispatchEvent(ev);
                    }
                    console.log(JSON.stringify(testResult));
                  }, testData, linkRequest, testResult);
                }
              }, 5000, linkRequest, testResult);
            } else {
              // TODO: send status too.
              // TODO: consume this on PHP.
              testResult.execute = 'Fail';
              testResult.message = 'Failed to load Page';
            }
        });
          setTimeout(function(){
            console.log("Finished Test\n");
            console.log(JSON.stringify(testResult) + "\n");
            responseCrafter(response, testResult);
      }, 20000, testResult);

    });
    if (!listening) {
        console.log("could not create web server listening on port " + port);
        phantom.exit();
    }
    var url = "http://localhost:" + port;
    console.log("SEND REQUEST TO:");
    console.log(url);
    //phantom.exit();
}
