<?php


 class Ecart_Orange extends CI_Controller {

        public function __construct(){
            parent::__construct();
            session_start();
            $this->load->model("Ecart_Orange_Model");

        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/ecart/ecart_orange", $this->getData());
        }       

        private function getData() {
            $data["ecart_boa"] = $this->Ecart_Orange_Model->get_ecart_boa();
            $data["ecart_orange"] = $this->Ecart_Orange_Model->get_ecart_orange();

            return $data;
        }
}



?>