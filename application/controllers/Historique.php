<?php
 class Historique extends CI_Controller {

        public function __construct(){
            parent::__construct();

        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/historique/historique");
        }
 }



?>