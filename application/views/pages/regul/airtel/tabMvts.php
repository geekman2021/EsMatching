


<div id="tabMvts" class="container-fluid mt-1 tab-pane "><br>
  <div class="row">
      <div class="col-sm-6 pl-4">
          <label for="dateDebut">Date Debut</label>
          <input type="text" class="form-control" id="minBoa" name="minBoa">
      </div>
      <div class="col-sm-6 pr-4">
        <label for="dateDebut">Date Fin</label>
          <input type="text" class="form-control" id="maxBoa" name="maxBoa">
      </div> 
  </div>
  <div class="container-fluid mt-4">
    <div class="card">
      <div class="card-header">
        <b>Liste MVTS Régularisé</b>
      </div>
      <div class="card-body">
      <table class="table table-bordered" id="tableMvts">
      <thead style="text-align: center ;">
          <tr>
            <th>External_id</th>
            <th>id_transfer </th>
            <th>Transfer_date</th>
            <th>Compte</th>
            <th>sender_msiSDN</th>
            <th>Dest_MsiSDN</th>
            <th>Montant</th>
            <th>Description</th>
            <th>Nom_Service</th>
            <th>Num_réference</th>
            <th>Action</th>
          </tr>
      </thead>
      <tbody>
        
      </tbody>
    </table>
  </div>
    </div>
  </div>
</div>

<?php $this->load->view("pages/regul/airtel/modal/modalMvts")?>