<?php
    class Droit_Model extends CI_Model {
        
        public function __construct() {
            parent:: __construct();
            $this->load->database("db1");
        }

        public function insert($data) {
            return $this->db->insert("droits", $data);
        }

        public function get_droit() {
            $query= $this->db->get("droits");
            return $query->result();
        }

        public function update($id, $droits) {
            $this->db->where("id", $id);
            return $this->db->update("droits", $droits);
        }

        public function delete($id) {
            $this->db->where("id", $id);
            return $this->db->delete("droits");
        }



    }

?>