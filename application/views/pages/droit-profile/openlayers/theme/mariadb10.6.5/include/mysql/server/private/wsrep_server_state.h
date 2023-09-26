this->_storeError($form_display, $error);

        $retval .= '<div id="' . $this->_elemId . '">';
        $retval .= '<div class="page_settings">';
        $retval .= $form_display->getDisplay(
            true,
            true,
            false,
            $response->getFooter()->getSelfUrl(),
            array(
                'submit_save' => $this->_groupName
            )
        );
        $retval .= '</div>';
        $retval .= '</div>';

        return $retval;
    }

    /**
     * Get HTML output
     *
     * @return string
     */
    public function getHTML()
    {
        return $this->_HTML;
    }

    /**
     * Get error HTML output
     *
     * @return string
     */
    public function getErrorHTML()
    {
        return $this->_errorHTML;
    }

    /**
     * Group to show for Page-related settings
     * @param string $formGroupName The name of config form group to display
     * @return PageSettings
     */
    public static function showGroup($formGroupName)
    {
        $object = new PageSettings($formGroupName);

        $response = Response::getInstance();
        $response->addHTML($object->getErrorHTML());
        $response->addHTML($object->getHTML());

        return $object;
    }

    /**
     * Get HTML for navigation settings
     * @return string
     */
    public static function getNaviSettings()
    {
        $object = new PageSettings('Navi', 'pma_navigation_settings');

        $response = Response::getInstance();
        $response->addHTML($object->getErrorHTML());
        return $object->getHTML();
    }
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          