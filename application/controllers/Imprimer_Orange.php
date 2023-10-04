<?php 

    class Imprimer_Orange extends CI_Controller {
        public function __construct() {
            parent::__construct();
            $this->load->model("check_model");
            $this->load->model("user_model");
            session_start();
        }

        public function index() {
            
        }

        public function imprimerDocument() {
            $i=0;

            $ref_igor = $this->input->get('ref');
            $compte= $this->input->get("compte");
            $montant= $this->input->get("montant");
            $oper= $this->input->get("oper");
            $date_oper= $this->input->get("date");
            $date= date("d/m/y");
            $annee= date("y");
            $reference= $i++;

            $template = $this->load->view('templates/frDixOrange', array(
                'ref_igor' => $ref_igor,
                'compte' => $compte,
                'montant' => $montant,
                'date_oper' => $date_oper,
                'oper' => $oper,
                'date' => $date,
                'annee' => $annee,
                "reference" => $reference

            ));
           
        }

    }


?>