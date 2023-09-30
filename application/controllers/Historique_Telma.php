<?php
 class Historique_Telma extends CI_Controller {

        public function __construct(){
            parent::__construct();

        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/historique/historique");
            $this->load->view("pages/historique/historique_telma");
        }
 }



?>