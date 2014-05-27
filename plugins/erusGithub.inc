<?php
/**
 * @file
 * ERUS GitHub plugin.
 *
 * Uses V3 of the github API.
 *
 */

class erusGithub extends erusPlugin {

  /**
   * Set the github api endpoint
   */
  public function __construct() {
    $this->set_endpoint('https://api.github.com/repos/{account}/{project}');
  }

  /**
   * The Magic Happens.
   * @return [type] [description]
   */
  public function process() {
    $data = $this->get_data();
    $info = $this->get_info();
    $client = $this->get_client();
    $request = $client->get('releases');
    $response = $request->send();

    $code = $response->getStatusCode();

    if ($code !== 200) {
      watchdog('erus', 'GitHub API returned with response code: ' . $code . ' for: ' . $this->get_project_name(), array(), WATCHDOG_ERROR);
      return false;
    }

    // GitHub gave us back a response.
    $github_data = $response->json();

    // If the github data is an array and not empty get the first element.
    if (!is_array($github_data) || empty($github_data)) {
      return false;
    }

    // Latest release.
    $latest = array_shift($github_data);
    // Convert GH data into update modules data.
    $available = $this->map_data($latest);
    // Allow update module to do its magic.
    update_calculate_project_update_status($this->get_project_name(), $data, $available);
    return $data;
  }

  /**
   * [get_client description]
   * @return [type] [description]
   */
  protected function get_client() {

    $end = $this->get_endpoint();
    $info = $this->get_info();

    $client = new Guzzle\Http\Client($end, array(
      'account' => $info['erus']['account'],
      'project' => $this->get_project_name(),
    ),
    array(
      'headers' => array('Accept' => 'application/vnd.github.v3+json'),
    ));

    return $client;
  }

  /**
   * Maps the GH response data into Update status usable format.
   * @param  [type] $latest_release [description]
   * @return [type]                 [description]
   */
  protected function map_data($latest_release) {
    $real_data = $this->get_data();
    $data = $this->get_default_update_status_available_data_array();
    $tag_name = $latest_release['tag_name'];
    $tag_parts = explode('-', $tag_name);

    $data['title'] =                $real_data['info']['name'];
    $data['short_name'] =           $real_data['name'];
    $data['type'] =                 $real_data['project_type'];
    $data['recommended_major'] =    $this->parse_latest_major($tag_name);
    $data['supported_majors'] =      $this->parse_latest_major($tag_name);
    $data['default_major'] =        $this->parse_latest_major($tag_name);
    $data['link'] =                 $latest_release['html_url'];
    $data['last_fetch'] =           time();

    $data['releases'][$tag_name] =  array(
      'name' => $real_data['name'] . ' ' . $tag_name,
      'version' => $tag_name,
      'tag' => $tag_name,
      'version_major' => $this->parse_latest_major($tag_name),
      'version_patch' => 0,
      'version_extra' => (isset($tag_parts[2])) ? $tag_parts[2] : '',
      'status' => 'published',
      'release_link' => $latest_release['html_url'],
      'download_link' => $latest_release['tarball_url'],
      'date' => strtotime($latest_release['published_at']),
      'mdhash' => md5($latest_release['url']),
      'filesize' => $latest_release['assets'][0]['size'],
    );

    return $data;
  }

}
