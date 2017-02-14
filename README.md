# omnitester
Automated tests system for Adobe Analytics.

## Getting Started

1. Install the module into a drupal website and enable it together with it's
dependencies

2. Install PhantomJS http://phantomjs.org/

3. Run the server.js script with phantomjs.
  -  Your command should be similar to this: phantomjs server.js [PORT] [AuthUser] [AuthPass]
  -  If you plan to run the tests on a HTTPS site, you need to add "--ignore-ssl-errors=yes" to the command. Example: phantomjs --ignore-ssl-errors=yes server.js 8001 user pass

4. Set up your Analytics testing sheet following the sheet template found in the repo. Check the contents of SheetTemplate.xls for some examples. It should be filled as follows:
  - The URL Column is where you should insert the URL for the test to run.(Required for both types of tests).
  - The Type Column is where you should insert the Type of the test you want to be executed for this row. (Can be Load or Link).
  - The LinkType Column is where you should insert the type of link you expect to be tracked (Only applicable for Link test type).
  - The PageName Column is where you should insert the page name you expect to be tracked (Only applicable for Load test type).
  - The Link Column is where you should insert the Link text you expect to be tracked (Only applicable for Link test type).
  - The Element is where you should insert the selector that will be clicked for link test types (Only applicable for Link test type).
  - The RequestIdentifier is where you should insert a string that identifies the tracking requests for the Analytics Implementation between all the requests sent from the page(Required for both types of tests).


###TODO:
1. The phantomjs script for the server always require user and pass for auth,
thats not always needed. It should be optional.
2. Only .xls files are being accepted, xlsx are generating errors.(Means you should save your sheet on 97-2003 excel format).
3. Search and implement all TODOs found in code.
4. Improve drupal module's .install file.
5. Review code for coding standards.
6. Make the drupal module generic enough for it to be uploaded to the community.
7. Add test for hover and scroll trackings.
