<?php
/**
 * API FUNCS
 */

/**
 * Return an array of plugin information
 * @return [type] [description]
 */
function hook_erus_plugin_info() {
  $plugins = array();

  $plugins['my_plugin'] = array(
    'file' => 'plugins/myExamplePlugin.inc',
    'name' => 'myExamplePluginName',
    'module' => 'my_module_name',
  );

  return $plugins;
}

/**
 * Alter plugin information
 * @param  [type] $plugins [description]
 * @return [type]          [description]
 */
function hook_erus_plugin_info_alter(&$plugins) {

}
