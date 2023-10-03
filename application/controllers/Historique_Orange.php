<?php
 class Historique_Orange extends CI_Controller {

        public function __construct(){
            parent::__construct();
            $this->load->model("historique_orange_model");
        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/historique/historique");
            $this->load->view("pages/historique/historique_orange", $this->get_data());
        }

        private function get_data() {
            $data["historique"] = $this->historique_orange_model->get_historique();
            return $data; 
        }


 }



?>