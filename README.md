# ERUS

### External Repository Update Status.

For connecting with external repository services such as github or bitbucket in
order to obtain update status information for custom work.


## INSTALLATION AND CONFIGURATION

Add the following to the .info file of the project that is hosted with an external repository service.

	erus[plugin] = plugin_name
	erus[account] = remote-account-name
	erus[project] = *optional_project_name*

eg: for https://github.com/SU-SWS/stanford_person

	erus[plugin] = github
	erus[account] = SU-SWS
	erus[project] = stanford_person

##API

	/**
	* Define your own plugins
	* /
	hook_erus_plugin_info()
	
	/**
	* Alter existing plugin information
	*/
	hook_erus_plugin_info_alter()

## TODO

 * Development versions
 * Non Drupal naming convention releases
 * Pre-releases should not be latest recommended.
 * Configuration Form
 * Oauth integration
 * Many many bug fixes and code clean up.

