<?php
/**
 * @file
 * Erus abstract plugin class
 */

use Guzzle\Http\Client;

abstract class erusPlugin implements erusPluginInterface {

  protected $response = null;
  protected $storage = array();
  protected $info = array();
  protected $data = array();
  protected $client = null;
  protected $endpoint = null;

  /**
   * [__construct description]
   */
  public function __construct() {

  }

  /**
   * The meat
   * @return [type]           [description]
   */
  public function process() {
    // You need to override this one.
  }

  /**
   * [set_data description]
   * @param [type] $settings [description]
   */
  public function set_data($data) {
    $this->data = $data;
    $this->set_info($data['info']);
  }

  /**
   * [get_data description]
   * @return [type] [description]
   */
  protected function get_data() {
    return $this->data;
  }

  /**
   * [set_info description]
   * @param [type] $settings [description]
   */
  public function set_info($info) {
    $this->info = $info;
  }

  /**
   * [get_info description]
   * @return [type] [description]
   */
  protected function get_info() {
    return $this->info;
  }

  /**
   * [set_storage description]
   * @param [type] $storage [description]
   */
  protected function set_storage($storage) {
    $this->storage = $storage;
  }

  /**
   * [get_storage description]
   * @return [type] [description]
   */
  protected function get_storage() {
    return $this->storage;
  }

  /**
   * Set the http client
   * @param [type] $client [description]
   */
  public function set_client($client) {
    $this->client = $client;
  }

  /**
   * [get_client description]
   * @return [type] [description]
   */
  protected function get_client() {
    return $this->client;
  }

  /**
   * [set_enpoint description]
   * @param [type] $endpoint [description]
   */
  protected function set_endpoint($endpoint) {
    $this->endpoint = $endpoint;
  }

  /**
   * [get_endpoint description]
   * @return [type] [description]
   */
  protected function get_endpoint() {
    return $this->endpoint;
  }

  /**
   * [get_project_name description]
   * @return [type] [description]
   */
  protected function get_project_name() {
    $info = $this->get_info();

    // If there is an erus override use that.
    if (isset($info['erus']['project'])) {
      return $info['erus']['project'];
    }

    // If there this a project name in the .info use that
    if (isset($info['project'])) {
      return $info['project'];
    }

    // Last ditch effort, create a name out of the name.
    $filter = array(' ' => '_', '/' => '_', '[' => '_', ']' => '');
    $name = strtolower($info['name']);

    return drupal_clean_css_identifier($name, $filter);
  }

  /**
   * Returns an array of default update status fields
   * @return [type]                [description]
   */
  protected function get_default_update_status_available_data_array() {
    return array(
      'title' => '',
      'short_name' => '',
      'type' => 'module',
      'api_version' => '7.x',
      'recommended_major' => '1',
      'supported_majors' => '1',
      'default_major' => '1',
      'project_status' => 'published',
      'link' => '',
      'terms' => '',
      'releases' => array(
        // 'tag_name' => array(
        //   'name' => '',
        //   'version' => '',
        //   'tag' => '',
        //   'version_major' => '',
        //   'version_patch' => '',
        //   'version_extra' => '',
        //   'status' => '',
        //   'release_link' => '',
        //   'download_link' => '',
        //   'date' => '',
        //   'mdhash' => '',
        //   'filesize' => '',
        //   'files' => '',
        //   'terms' => array(
        //     'Release type' => array(
        //       'New features', 'Bug fixes',
        //     )
        //   ),
        // )
      ),
      'last_fetch' => 0,
    );
  }

  /**
   * [parse_latest_major description]
   * @param  [type] $tag_name [description]
   * @return [type]           [description]
   */
  protected function parse_latest_major($tag_name) {

    // Figure out what the currently installed major version is. We need
    // to handle both contribution (e.g. "5.x-1.3", major = 1) and core
    // (e.g. "5.1", major = 5) version strings.
    $matches = array();
    if (preg_match('/^(\d+\.x-)?(\d+)\..*$/', $tag_name, $matches)) {
      return $matches[2];
    }

  }

}
