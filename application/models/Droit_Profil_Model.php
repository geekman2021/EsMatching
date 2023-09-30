<?php
class Droit_Profil_Model extends CI_Model {
    public function __construct() {
        parent:: __construct();
        $this->load->database("default");
    }

    public function insert($data) {
        return $this->db->insert("droit_profils", $data);
    }

    public function get_droits_profile() {
        $this->db->select("*");
        $this->db->from("droit_profils");
        $this->db->order_by("code_profil", "ASC");
        $query= $this->db->get();
        return $query->result();
    }

    public function update($id, $droits_profils) {
        $this->db->where("id", $id);
        return $this->db->update("droit_profils", $droits_profils);
    }

    public function delete($id) {
        $this->db->where("id", $id);
        return $this->db->delete("droit_profils");
    }

}

?>