<?php
    class Regul extends CI_Controller {

        public function __construct() {
            parent::__construct();
            session_start();
        
        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/regul/regul");
        }
        
    }
    



?>