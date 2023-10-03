<?php
 class Historique_Telma extends CI_Controller {

        public function __construct(){
            parent::__construct();
            $this->load->model("historique_telma_model");

        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/historique/historique");
            $this->load->view("pages/historique/historique_telma", $this->get_data());
        }

        public function get_data() {
            $data["historique"] = $this->historique_telma_model->get_historique();

            return $data;
            
        }
 }



?>