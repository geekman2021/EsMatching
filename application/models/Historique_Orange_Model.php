<?php
    
    class Historique_Orange_Model extends CI_Model {
        public function __construct() {
            parent::__construct();
            $this->load->database("default");
        }

        
        public function insert($data) {
            if(array_key_exists("princ_ref_igor", $data)) {
                $this->db->where("princ_ref_igor", $data["princ_ref_igor"]);
            } if(array_key_exists("orange_ref", $data)) {
                $this->db->where("orange_ref", $data["orange_ref"]);
            }

            $exist= $this->db->get("historique_orange");

            if($exist->num_rows() == 0) {
                $this->db->insert("historique_orange", $data);
            } else {
                if(array_key_exists("princ_ref_igor", $data)) {
                    $this->db->where("princ_ref_igor", $data["princ_ref_igor"]);
                    return $this->db->update("historique_orange", $data);
                } if(array_key_exists("orange_ref", $data)) {
                    $this->db->where("orange_ref", $data["orange_ref"]);
                    return $this->db->update("historique_orange", $data);
                }
            }
        }

        public function get_historique() {
            $this->db->select("*");
            $this->db->from("historique_orange");
            $this->db->order_by("id", "ASC");
            $query= $this->db->get();

            return $query->result();
        }

        public function get_last_solde() {
            $sql= "SELECT princ_solde, comm_solde, solde, orange_solde FROM historique_orange ORDER BY id DESC LIMIT 1";

            $query= $this->db->query($sql);

            return $query->result();
        }

        public function get_solde_by_month() {
            $resultArray = array();

            for ($i = 1; $i <= 12; $i++) {
                $sql = "SELECT princ_date_oper, orange_date,  COALESCE(princ_montant,0) AS principale, COALESCE(orange_montant,0) AS orange, COALESCE(comm_montant, 0) AS commission FROM historique_orange WHERE MONTH(princ_date_oper)=$i OR MONTH(orange_date)=$i";
                $query = $this->db->query($sql);
                if ($query->num_rows() > 0) {
                    $resultArray[$i] = $query->result_array();
                } else {   
                    $resultArray[$i] = array(
                        'princ_date_oper' => 0,
                        'orange_date' => 0,
                        'princ_montant' => 0,
                        'comm_montant' => 0,
                        "orange_montant" =>0
                    );
                }
            } 
            return $resultArray;
        }

        


    }


?>