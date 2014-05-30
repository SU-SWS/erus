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
  public function __construct($settings) {
    parent::__construct($settings);
    $this->set_endpoint('https://api.github.com/repos/{account}/{project}');
  }

  /**
   * The Magic Happens.
   * @param  $data  [description]
   * @return [type] [description]
   */
  public function process($data) {

    // Store data for later
    $this->set_update_data($data);

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

    $releases = $github_data;
    // Convert GH data into update modules data.
    $available = $this->map_data($releases);
    // Sanitize the status of the data from the original call
    $this->sanitize_info_data($data);
    // Allow update module to do its magic.
    update_calculate_project_update_status($this->get_project_name(), $data, $available);

    // Small hack to show update is available instead of unsupported...
    if ($data['status'] == 3) {
      $data['status'] = 4;
    }

    return $data;
  }

  /**
   * [get_client description]
   * @return [type] [description]
   */
  protected function get_client() {

    $end = $this->get_endpoint();

    $client = new Guzzle\Http\Client($end, array(
      'account' => $this->get_account(),
      'project' => $this->get_project_name(),
    ),
    array(
      'headers' => array('Accept' => 'application/vnd.github.v3+json'),
    ));

    return $client;
  }

  /**
   * Maps the GH response data into Update status usable format.
   * @param  [type] $releases [description]
   * @return [type]                 [description]
   */
  protected function map_data($releases) {
    $update_data = $this->get_update_data();
    $data = $this->get_default_update_status_available_data_array();
    $latest_release = $this->get_latest_release($releases);
    $latest_prod_tag = !empty($latest_release['tag_name']) ? $latest_release['tag_name'] : '';

    $data['title'] =                $update_data['info']['name'];
    $data['short_name'] =           $update_data['name'];
    //$data['type'] =                 $update_data['project_type'];
    $data['link'] =                 $this->get_project_url($latest_release);
    $data['last_fetch'] =           time();

    $data['recommended_major'] =    $this->parse_latest_major($latest_prod_tag);
    $data['supported_majors'] =     implode(",", range(1,99));
    $data['default_major'] =        $this->parse_latest_major($latest_prod_tag);

    foreach ($releases as $index => $release) {
      $tag_name = $release['tag_name'];
      $tag_parts = explode('-', $tag_name);

      $data['releases'][$tag_name] =  array(
        'name' => $update_data['name'] . ' ' . $tag_name,
        'version' => $tag_name,
        'tag' => $tag_name,
        'version_major' => $this->parse_latest_major($tag_name),
        'version_patch' => 0,
        'version_extra' => ($release['prerelease']) ? 'dev' : $tag_parts[2],
        'status' => 'published',
        'release_link' => $release['html_url'],
        'download_link' => $release['tarball_url'],
        //'date' => strtotime($release['published_at']), // when the release was published.
        'date' => strtotime($release['created_at']), // when the tag was created
        'mdhash' => md5($release['url']), // just making this stuff up.
        'filesize' => isset($release['assets'][0]) ? $release['assets'][0]->size : 0,
        'files' => "\n  \n  \n  ",
        'terms' => array('Release type' => array('Bug fixes')),
      );

    }

    return $data;
  }

  /**
   * Goes through the releases looking for a release that isnt marked as
   * a "pre-lease" in the github ui.
   * @param  [array] $releases [all of the releases from GH]
   * @return [array]           [the latest mared as non prerelease]
   */
  protected function get_latest_release($releases) {

    foreach ($releases as $i => $release) {
      if ($release['draft'] == TRUE || $release['prerelease'] == TRUE) {
        continue;
      }
      return $release;
    }

  }

  /**
   * Looks for the latest tag marked with pre-release
   * @param  [type] $releases [description]
   * @return [type]           [description]
   */
  protected function get_latest_dev_release($releases) {
    foreach ($releases as $i => $release) {
      if ($release['prerelease'] == TRUE) {
        return $release;
      }
    }
  }

  /**
   * Return a link to the github page.
   * @param  [type] $releases [description]
   * @return [type]           [description]
   */
  protected function get_project_url($release) {
    $bits = parse_url($release['html_url']);
    $its_n_bits = explode("/", ltrim($bits['path'], "/"));
    return $bits['scheme'] . "://" . $bits['host'] . "/" . $its_n_bits[0] . "/" . $its_n_bits[1];
  }

}
