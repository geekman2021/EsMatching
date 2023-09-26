<?php
    class Telma extends CI_Controller {
        
        public function __construct(){
            parent::__construct();
        }

        public function index() {
            $this->load->view("templates/header");
            $this->load->view("pages/operateur/telma-form");
            $this->load->view("templates/footer");
        }
    }
?>