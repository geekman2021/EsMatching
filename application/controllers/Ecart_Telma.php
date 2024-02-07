<?php

require 'vendor/autoload.php';
use PhpOffice\PhpSpreadsheet\Spreadsheet;

 class Ecart_Telma extends CI_Controller {

    public function __construct(){
        parent::__construct(); // Set the memory limit to 256 megabytes
        session_start();
        $this->load->model("Ecart_Telma_Model");
    }

    public function index() {
        $this->load->view("templates/sidebar");
        $this->load->view("pages/ecart/ecart_telma", $this->get_data());
    }      

    private function get_data() {
        $data["boa"]= $this->Ecart_Telma_Model->get_ecart_boa();
        $data["telma"]= $this->Ecart_Telma_Model->get_ecart_telma();

        return $data;
    }
}



?>