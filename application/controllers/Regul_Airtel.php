<?php
    class Regul_Airtel extends CI_Controller {

        public function __construct() {
            parent::__construct();
            $this->load->model("Regul_Airtel_Model");
            session_start();
            
            
        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/regul/regul");
            $this->load->view("pages/regul/regul_airtel", $this->get_data());
        }

        public function get_data() {

            $data["ci"] = $this->Regul_Airtel_Model->get_regul_ci();
            $data["ambi"] = $this->Regul_Airtel_Model->get_ambi();
            $data["ambi_co"] = $this->Regul_Airtel_Model->get_ambi_co();
            $data["deallo_vi"] = $this->Regul_Airtel_Model->get_deallo_vi();
            $data["vi_deallo"] = $this->Regul_Airtel_Model->get_vi_deallo();
            return $data;
        }
        
    }
    



?>