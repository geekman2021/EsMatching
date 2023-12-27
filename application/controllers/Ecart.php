<?php
 class Ecart extends CI_Controller {

        public function __construct(){
            parent::__construct(); // Set the memory limit to 256 megabytes
            session_start();
        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/ecart/ecart");
        }

        public function exporterVersExcel(){
           
        }
}



?>