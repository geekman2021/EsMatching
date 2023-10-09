<?php 

    class Imprimer extends CI_Controller {
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
            $compte_client= $this->input->get("compte_client");
            $compte_comm= $this->input->get("compte_comm");
            $compte= $this->input->get("compte");
            $nom= $this->input->get("nom");
            $responsable= $this->input->get("responsable");
            $operateur= $this->input->get("operateur");
            $mobile_money= $this->input->get("mobile_money");
            $libelle= $this->input->get("libelle");
            $montant= $this->input->get("montant");
            $oper= $this->input->get("oper");
            $date_oper= $this->input->get("date");
            $tax= $this->input->get("tax");
            $tva= $this->input->get("tva");
            $montant_comm= $this->input->get("montant_comm");
            $place = $this->input->get("place");

            $date= date("d/m/y");
            $annee= date("y");
            $reference= $i++;

            $template = $this->load->view('templates/frDix', array(
                'ref_igor' => $ref_igor,
                'compte' => $compte,
                'compte_client' => $compte_client,
                'compte_comm' => $compte_comm,
                'nom' => $nom,
                'responsable' => $responsable,
                'operateur' => $operateur,
                'mobile_money' => $mobile_money,
                'montant' => $montant,
                'libelle' => $libelle,
                'date_oper' => $date_oper,
                'oper' => $oper,
                'date' => $date,
                'annee' => $annee,
                "reference" => $reference,
                'tva' => $tva,
                'tax' => $tax,
                'montant_comm' => $montant_comm,
                "place" => $place
            ));
           
        }

    }


?>