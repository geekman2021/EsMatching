<?php

    

    class Regul_Airtel extends CI_Controller {

        public function __construct() {
            parent::__construct();
            
            
        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/regul/regul");
            $this->load->view("pages/regul/regul_airtel");

        }
        
    }
    



?>