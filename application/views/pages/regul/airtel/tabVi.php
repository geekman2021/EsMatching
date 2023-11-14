<div id="tabVi" class="container-fluid mt-5 tab-pane ">
    <div class="row">
      <div class="col-sm-6 pl-4">
          <label for="dateDebut">Date Debut</label>
          <input type="text" class="form-control" id="minAirtel" name="minAirtel">
      </div>
        <div class="col-sm-6 pr-4">
          <label for="dateDebut">Date Fin</label>
          <input type="text" class="form-control" id="maxAirtel" name="maxAirtel">
        </div> 
    </div>
    <div class="container-fluid mt-3">
      <div class="card">
        <div class="card-header">
          <b>Liste VI Régularisé</b>
        </div>
        <div class="card-body">
        <table class="table table-bordered" id="tableVi">
        <thead>
            <tr>
              <th>Compte</th>
              <th>Date_Oper</th>
              <th>Date_Val </th>
              <th>Devise</th>
              <th>Montant</th>
              <th>Libelle</th>
              <th>Operation</th>
              <th>Expl</th>
              <th>ReferenceIgor</th>
              <th>Action</th>
            </tr>
        </thead>
        <tbody>
          <?php if(!empty($deallo_vi)) { ?>
        <?php foreach($deallo_vi as $item) { ?>
          <tr>
            <td><?= $item->COMPTE ?></td>
            <td><?= $item->DATE_OPER ?></td>
            <td><?= $item->DATE_VAL ?></td>
            <td><?= $item->DEVISE ?></td>
            <td><?= $item->MONTANT ?></td>
            <td><?= $item->LIBELLE ?></td>
            <td><?= $item->OPER ?></td>
            <td><?= $item->EXPL ?></td>
            <td><?= $item->REF_IGOR ?></td>
            <td style="text-align: center;"><a href="#" data-toggle="modal" data-target="#myModalVi<?= $item->id_deallo ?>"><i class="bx bx-show" style="font-size: 20px"></i></a></td>
          </tr>
        <?php } ?>
      <?php } ?>
        </tbody>
    </table>
        </div>
      </div>
    </div>
  </div>
</div>
<?php $this->load->view("pages/regul/airtel/modal/modalVi")?>