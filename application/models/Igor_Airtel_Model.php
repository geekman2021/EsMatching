<?php
    class Igor_Airtel_Model extends CI_Model {

        public function __construct() {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_or_update_ci($data) {

                $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
                $exist= $this->db->get("airtel_igor_normale_ci");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("airtel_igor_normale_ci", $data);
                } else {
                    $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
                    return $this->db->update("airtel_igor_normale_ci", $data);
                }

            
        }

        public function insert_or_update_co($data) {

        
                $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
                $exist= $this->db->get("airtel_igor_normale_co");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("airtel_igor_normale_co", $data);
                } else {
                    $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
                    return $this->db->update("airtel_igor_normale_co", $data);
                }

            
        }



        public function get_ci() {
            $this->db->select("*");
            $this->db->from("airtel_igor_normale_ci");
            $query= $this->db->get();

            return $query->result();
        }

        public function get_co() {
            $this->db->select("*");
            $this->db->from("airtel_igor_normale_co");
            $query= $this->db->get();

            return $query->result();
        }

        public function insert_or_update_nonAu($data) {
            $this->db->where("ref_igor", $data["ref_igor"]);
            $exist= $this->db->get("boa_airtel_nonau");

            if($exist->num_rows() == 0) {
                return $this->db->insert("boa_airtel_nonau", $data);
            } else {
                $this->db->where("ref_igor", $data["ref_igor"]);
                return $this->db->update("boa_airtel_nonau", $data);
            }
        }


        public function insert_or_update_deallo_vi($data) {
            $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
            $exist= $this->db->get("deallo_vi");
            if($exist->num_rows() == 0) {
                return $this->db->insert("deallo_vi", $data);
            } else {
                $this->db->where("TRANSFER_ID", $data["TRANSFER_ID"]);
                return $this->db->update("deallo_vi", $data);
            }
        }

        public function get_deallo_vi() {
            $this->db->select("*");
            $this->db->from("deallo_vi");
            $query= $this->db->get();
        }

        

    
    }

?>