<?php
 class Historique_Airtel extends CI_Controller {

        public function __construct(){
            parent::__construct();
            $this->load->model("historique_airtel_model");

        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/historique/historique");
            $data["historique"] = $this->historique_airtel_model->get_historique();
            $this->load->view("pages/historique/historique_airtel", $data);
        }
 }



?>