<?php 
    class Principal_Model extends CI_Model {

        public function __construct()
        {
            parent::__construct();
            $this->load->database("db1");
        }

        public function insert_or_update($data) {

            $this->db->where("ref_igor", $data["ref_igor"]);
            $exist= $this->db->get("principal");
            if($exist->num_rows() == 0) {
                return $this->db->insert("principal", $data);
            } else {
                $this->db->where("ref_igor", $data["ref_igor"]);
                return $this->db->update("principal", $data);
            }

        }
    }
?>