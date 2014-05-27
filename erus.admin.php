<?php
/**
 * @file
 * Administrative interface functionality
 */

/**
 * [erus_configuration_form description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function erus_configuration_form($form, &$form_state) {

  $form['somethingsoemthing']['#markup'] = "<p>Create a form that the plugins can hook into and add their settings to.</p>";

  return $form;
}
