<?php

    class Importer extends CI_Controller {
    

        public function __construct(){
            parent::__construct();
            session_start();
        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/importer/importer");

        }

        public function importer() {
            


        }


       

    }

?>