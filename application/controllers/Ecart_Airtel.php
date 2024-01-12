<?php

require 'vendor/autoload.php';
use PhpOffice\PhpSpreadsheet\Spreadsheet;

 class Ecart_Airtel extends CI_Controller {

    public function __construct(){
        parent::__construct(); // Set the memory limit to 256 megabytes
        session_start();
        $this->load->model("Ecart_Airtel_Model");
    }

    public function index() {
        $this->load->view("templates/sidebar");
        $this->load->view("pages/ecart/ecart_airtel", $this->getData());
    }
    
    private function getData() {
        $data["ecart_airtel"] = $this->Ecart_Airtel_Model->get_ecart_airtel();
        $data["ecart_boa"] = $this->Ecart_Airtel_Model->get_ecart_boa();
        return $data;
    }
}



?>