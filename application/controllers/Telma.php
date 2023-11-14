<?php

    defined('BASEPATH') OR exit('No direct script access allowed');

    class Telma extends CI_Controller {
        public function __construct(){
            parent::__construct();
            $this->load->model("Telma_Normal_Model");
            $this->load->model("Telma_Anomalie_Model");
            $this->load->model("Boa_Telma_Anomalie_Model");
            session_start();
        }
        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/operateur/telma-list", $this->get_data());
        }

        public function get_data() {

            $data["normal_ci"] = $this->load->Telma_Normal_Model->get_ci();
            $data["normal_co"] = $this->load->Telma_Normal_Model->get_co();
            $data["vi"] = $this->load->Boa_Telma_Anomalie_Model->get_vi();
            $data["admin"] = $this->Telma_Anomalie_Model->get_admin();
            $data["dat"] = $this->Boa_Telma_Anomalie_Model->get_ci();
            $data["cat"] = $this->Boa_Telma_Anomalie_Model->get_co();


            return $data;

        }
        
    }


    ?>



