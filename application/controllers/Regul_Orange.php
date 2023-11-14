<?php

    

    class Regul_Orange extends CI_Controller {

        public function __construct() {
            parent::__construct();
            $this->load->model("Regul_Telma_Model");
            session_start();


        }

        public function index() {

            $this->load->view("templates/sidebar");
            $this->load->view("pages/regul/regul");
            $this->load->view("pages/regul/regul_orange", $this->get_data());

        }

        public function get_data() {

            $data["ci"]= $this->Regul_Telma_Model->get_regul_ci();
            return $data;
        }

    
        
    }
    



?>