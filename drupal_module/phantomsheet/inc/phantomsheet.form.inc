<?php

/**
 * @file
 * Phantomsheet Module Run test Form.
 */

/**
 * Form for setting up request settings.
 */
function phantomsheet_run_test_form($form, &$form_state) {
  $form['phantomsheet_sheet'] = array(
    '#type' => 'managed_file',
    '#title' => t('Test Sheet'),
    '#description' => t('Upload test sheet'),
    '#upload_location' => 'public://phantomsheet',
    '#upload_validators' => array(
      'file_validate_extensions' => array('xls xlsx'),
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
    '#ajax' => array(
      'callback' => 'phantomsheet_ajax_run_test',
      'wrapper' => 'gvision-all-functions-block-form',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  return $form;
}

/**
 * Submit function for testing form.
 */
function phantomsheet_ajax_run_test($form, &$form_state) {
  $fid = $form_state['values']['phantomsheet_sheet'];
  $file = file_load($fid);

  module_load_include('inc', 'phpexcel');
  $sheet = phpexcel_import($file->uri);

  // TODO: add url field here.
   $url = 'http://localhost:8001';

   $options = array(
       'http' => array(
           'header'  => "Content-type: application/x-www-form-urlencoded\r\n",
           'method'  => 'POST',
       )
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
    $options['http']['content'] = http_build_query($row);

    $context  = stream_context_create($options);
    $result = json_decode(file_get_contents($url, false, $context));

    $test_status = 'Fail';

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

    //TODO: Improve the verification of not Applicable and not found.
    $table['rows'][] = array(
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
    ajax_command_replace('#content', $test),
  );

  $replace = array('#type' => 'ajax', '#commands' => $commands);

  return $replace;

}
