<?php 
    class Airtel_Normal_Model extends CI_Model {

        public function __construct()
        {
            parent::__construct();
            $this->load->database("db1");
        }

        public function insert_or_update_ci($data) {

                $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
                $exist= $this->db->get("airtel_normale_ci");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("airtel_normale_ci", $data);
                } else {
                $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
                
                return $this->db->update("airtel_normale_ci", $data);
            }
        }

        public function insert_or_update_co($data) {

            $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
                $exist= $this->db->get("airtel_normale_co");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("airtel_normale_co", $data);
                } else {
                $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
                
                return $this->db->update("airtel_normale_co", $data);
            }
        }

        public function get_normal_ci() {
            $this->db->select("*");
            $this->db->from("airtel_anomalie_ci");
            $query= $this->db->get();

            return $query->result();
        }

        public function get_normal_co() {
            $this->db->select("*");
            $this->db->from("airtel_anomalie_co");
            $query= $this->db->get();

            return $query->result();
        }


        
    }
?>