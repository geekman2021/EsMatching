<?php

    class Importer_Orange extends CI_Controller {

        public $ambiguous= array();
        public $deallocation= array();
        public $airtel_anomalie_ci= array();
        public $airtel_anomalie_co= array();
        public $igor_anomalie_ci= array();
        public $igor_anomalie_co= array();


       
        public function __construct(){
            parent::__construct();
            
        }

        public function index() {

            $this->load->view("templates/sidebar");
            $this->load->view("pages/importer/importer");
            $this->load->view("pages/operateur/orange-form");
        }

        public function importer() {
            


        }


       

    }

?>