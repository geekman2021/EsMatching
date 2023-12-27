<?php 
    class Telma_Anomalie_Model extends CI_Model {

        public function __construct()
        {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_or_update_reverse($data) {

                $this->db->where("trans_id", $data["trans_id"]);
                $exist= $this->db->get("telma_reverse");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("telma_reverse", $data);
                } else {
                $this->db->where("trans_id", $data["trans_id"]);
                
                return $this->db->update("telma_reverse", $data);
            }
        }

        public function insert_or_update_admin($data) {

            $this->db->where("trans_id", $data["trans_id"]);
                $exist= $this->db->get("telma_admin");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("telma_admin", $data);
                } else {
                $this->db->where("trans_id", $data["trans_id"]);
                
                return $this->db->update("telma_admin", $data);
            }
        }
        public function insert_or_update_rollback($data) {

            $this->db->where("trans_id", $data["trans_id"]);
                $exist= $this->db->get("telma_rollback");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("telma_rollback", $data);
                } else {
                $this->db->where("trans_id", $data["trans_id"]);
                
                return $this->db->update("telma_rollback", $data);
            }
        }

        public function insert_or_update_mvts_ci($data) {
            
            $this->db->where("trans_id", $data["trans_id"]);
                $exist= $this->db->get("telma_mvts_ci");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("telma_mvts_ci", $data);
                } else {
                $this->db->where("trans_id", $data["trans_id"]);
                return $this->db->update("telma_mvts_ci", $data);
            }
        }
        public function insert_or_update_mvts_co($data) {

            $this->db->where("trans_id", $data["trans_id"]);
                $exist= $this->db->get("telma_mvts_co");
                if($exist->num_rows() == 0) {
                    return $this->db->insert("telma_mvts_co", $data);
                } else {
                $this->db->where("trans_id", $data["trans_id"]);
                
                return $this->db->update("telma_mvts_co", $data);
            }
        }

        public function get_admin() {
            $this->db->select("*");
            $this->db->from("telma_admin");
            $query= $this->db->get();

            return $query->result();
        }

        public function get_reverse() {
            $this->db->select("*");
            $this->db->from("boa_telma_normale_co");
            $query= $this->db->get();

            return $query->result();
        }

    }
?>