<?php

    

    class Regul_Telma extends CI_Controller {

        public function __construct() {
            parent::__construct();
            
            
        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/regul/regul_telma");

        }
        
    }
    



?>