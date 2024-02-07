<?php
    
    class Historique_Telma_Model extends CI_Model {
        public function __construct() {
            parent::__construct();
            $this->load->database("default");
        }

        
        public function insert_or_update($data) {
            if(array_key_exists("REF_IGOR", $data)) {
                $this->db->where("REF_IGOR", $data["REF_IGOR"]);
            } if(array_key_exists("trans_id", $data)) {
                $this->db->where("trans_id", $data["trans_id"]);
            }

            $exist= $this->db->get("historique_telma");

            if($exist->num_rows() == 0) {
                $this->db->insert("historique_telma", $data);
            } else {
                if(array_key_exists("REF_IGOR", $data)) {
                    $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                    return $this->db->update("historique_telma", $data);
                } if(array_key_exists("trans_id", $data)) {
                    $this->db->where("trans_id", $data["trans_id"]);
                    return $this->db->update("historique_telma", $data);
                }
            }
        }

        public function get_historique() {
            $this->db->select("*");
            $this->db->from("historique_telma");
            $this->db->order_by("id", "ASC");
            $query= $this->db->get();

            return $query->result();
        }

        public function get_last_solde() {
            $sql= "SELECT solde_telma, solde_boa FROM historique_telma ORDER BY id DESC LIMIT 1";

            $query= $this->db->query($sql);

            return $query->result();
        }

        public function get_solde_by_month() {
            $resultArray = array();
                for ($i = 1; $i <= 12; $i++) {
                    $sql = "SELECT date, COALESCE(solde,0) AS solde, COALESCE(MONTANT,0) AS MONTANT FROM `historique_telma` WHERE MONTH(date)=$i";
                    $query = $this->db->query($sql);
                    if ($query->num_rows() > 0) {
                        $resultArray[$i] = $query->result_array();
                    } else {
                        $resultArray[$i] = array(
                            'transfer_date' => 0,
                            'solde' => 0,
                            'MONTANT' => 0
                        );
                    }
                }
                return $resultArray;
        }
    }


?>