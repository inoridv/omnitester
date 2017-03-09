<?php

/**
 * @file
 * Phantomsheet Module Run test Form.
 */

/**
 * Form for setting up request settings.
 */
function phantomsheet_run_test_form($form, &$form_state) {

  $form = array();

  // TODO: improve this.
  $form["phantomsheet_message_wrapper"] = array("#markup" => "<div id='ajax-form-messages'></div>");

  $form['phantomsheet_sheet'] = array(
    '#type' => 'managed_file',
    '#title' => t('Test Sheet'),
    '#description' => t('Upload test sheet'),
    '#required' => TRUE,
    '#upload_location' => 'public://phantomsheet',
    '#upload_validators' => array(
      'file_validate_extensions' => array('xls xlsx'),
    ),
  );

  $form['phantomsheet_test_server_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Test server URL'),
    '#required' => TRUE,
  );

  // Add a fieldset here "website info".

  $form['phantomsheet_website_base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Website Base URL'),
    '#description' => t('Base URL for the website in which the tests will run. Please make sure to add the HTTP/HTPPS protocol.'),
    '#required' => TRUE,
  );

  $form['phantomsheet_website_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Website Username'),
    '#description' => t('Username for websites that require http(s) authentication.'),
  );

  $form['phantomsheet_website_password'] = array(
    '#type' => 'textfield',
    '#title' => t('Website Password'),
    '#description' => t('Password for websites that require http(s) authentication.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
    '#ajax' => array(
      'callback' => 'phantomsheet_ajax_run_test',
      'wrapper' => 'ajax-form-messages',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  $form["phantomsheet_result_wrapper"] = array("#markup" => "<div id='ajax-result-wrapper'></div>");

  return $form;
}

/**
 * Submit function for testing form.
 */
function phantomsheet_ajax_run_test($form, &$form_state) {

  // TODO: check for security issues.
  $fid = $form_state['values']['phantomsheet_sheet'];
  $test_server = $form_state['values']['phantomsheet_test_server_url'];

  $website_base_url = $form_state['values']['phantomsheet_website_base_url'];
  // There is no problem if it's empty. << remove later.
  $website_username = $form_state['values']['phantomsheet_website_username'];
  $website_password = $form_state['values']['phantomsheet_website_password'];

  $file = file_load($fid);

  module_load_include('inc', 'phpexcel');
  $sheet = phpexcel_import($file->uri);

   $options = array(
      'http' => array(
          'header'  => "Content-type: application/x-www-form-urlencoded\r\n",
          'method'  => 'POST',
      ),
   );

   $table = array(
     'header' => array('Type', 'Expected Linktype', 'LinkType Found', 'Expected Link', 'Link Found', 'Expected PageName', 'Pagename Found', 'Status'),
     'attributes' => array(),
     'caption' => 'Test Results',
     'colgroups' => array(),
     'sticky' => array(),
     'empty' => 'Not Applicable',
   );

  foreach($sheet[0] as $row) {
    $row['base_url'] = $website_base_url;
    $row['username'] = $website_username;
    $row['password'] = $website_password;
    $options['http']['content'] = http_build_query($row);

    $context  = stream_context_create($options);
    $result = json_decode(file_get_contents($test_server, false, $context));
    dpm($result);

    $test_status = 'Fail';

    // This won't check for element before acessing, so we have some E_NOTICEs.
    // not sure if its a problem.
    if ($result->Type == 'Link') {
      if ($result->Link->status == 'Pass' && $result->LinkType->status == 'Pass') {
        $test_status = 'Pass';
      }
    }
    else {
      if ($result->PageName->status == 'Pass') {
        $test_status = 'Pass';
      }
    }

    dpm($result);

    //TODO: Improve the verification of not Applicable and not found.
    $table['rows'][] = array(
      // This won't check for element before acessing, so we have some E_NOTICEs.
      // not sure if its a problem.
      $result->Type ? $result->Type : 'Not Applicable/Not Found',
      $result->LinkType->expected ? $result->LinkType->expected : 'Not Applicable/Not Found',
      $result->LinkType->found ? $result->LinkType->found : 'Not Applicable/Not Found',
      $result->Link->expected ? $result->Link->expected : 'Not Applicable/Not Found',
      $result->Link->found ? $result->Link->found : 'Not Applicable/Not Found',
      $result->PageName->expected ? $result->PageName->expected : 'Not Applicable/Not Found',
      $result->PageName->found ? $result->PageName->found : 'Not Applicable/Not Found',
      $test_status
    );
  }

  $test = theme_table($table);
  $commands = array(
    ajax_command_replace('#ajax-result-wrapper', $test),
  );

  $replace = array('#type' => 'ajax', '#commands' => $commands);

  return $replace;

}
