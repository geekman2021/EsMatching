<?php

    

    class Regul_Telma extends CI_Controller {

        public function __construct() {
            parent::__construct();

            $this->load->model("regul_telma_model");
            
        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/regul/regul");
            $this->load->view("pages/regul/regul_telma", $this->get_data());

        }

        public function get_data() {

            $data["dat"]= $this->regul_telma_model->get_regul_ci();
            return $data;
        }
        
    }
    



?>