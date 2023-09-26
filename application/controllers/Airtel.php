<?php

    defined('BASEPATH') OR exit('No direct script access allowed');
    // require "vendor/autoload.php";
    

    class Airtel extends CI_Controller {
        public function __construct(){
            parent::__construct();
            // session_start();
            // $this->load->library("PHPExcel");
            // $this->load->library("excel");
            // $this->load->model("airtel_model");
            // // $this->load->model("airtel_normal_model");
            // $this->load->model("airtel_anomalie_model");
            // // $this->load->model("igor_airtel_normal_model");
            // $this->load->model("igor_airtel_anomalie_model");
            // $this->load->model("igor_airtel_model");
            // $this->load->model("airtel_model");


        }
        public function index() {
            
            $this->load->view("templates/sidebar");

            // $data["anomalie_ci"] = $this->airtel_anomalie_model->get_anomalie_ci();
            // $data["anomalie_co"] = $this->airtel_anomalie_model->get_anomalie_co();
            // $data["igor_anomalie_ci"]= $this->igor_airtel_anomalie_model->get_anomalie_ci();
            // $data["igor_anomalie_co"]= $this->igor_airtel_anomalie_model->get_anomalie_co();
            // $data["deallocation"] = $this->airtel_anomalie_model->get_deallocation();
            // $data["ambiguous"] = $this->airtel_anomalie_model->get_ambiguous();
            // $data["normale_ci"]= $this->igor_airtel_model->get_ci();
            // $data["normale_co"] = $this->igor_airtel_model->get_co();
            // $data["vi"] = $this->igor_airtel_anomalie_model->get_vi();
            // $this->load->view("pages/operateur/airtel-form");
            $this->load->view("pages/operateur/airtel-list");
        }

        // public function regularisation 

    

        public function edit_anomalie_CO() {

            $id= $this->input->post("id");
            $etat= $this->input->post("etat");
            $date_regul= $this->input->post("date_regul");
            $this->airtel_anomalie_model->update_anomalie_co($id, $etat, $date_regul);

        }

        public function edit_anomalie_CI() {
            $id= $this->input->post("id");
            $etat= $this->input->post("etat");
            $date_regul= $this->input->post("date_regul");
            $this->airtel_anomalie_model->update_anomalie_CI($id, $etat, $date_regul);

        }

        public function edit_anomalie_deallo() {
            $id= $this->input->post("id");
            $etat= $this->input->post("etat");
            $date_regul= $this->input->post("date_regul");

            $this->airtel_anomalie_model->update_anomalie_deallo($id, $etat, $date_regul);
    
        }
    }


    ?>



