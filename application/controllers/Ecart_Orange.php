<?php

require 'vendor/autoload.php';
use PhpOffice\PhpSpreadsheet\Spreadsheet;

 class Ecart_Orange extends CI_Controller {

        public function __construct(){
            parent::__construct(); // Set the memory limit to 256 megabytes
            session_start();
        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/ecart/ecart_orange");
        }       
}



?>